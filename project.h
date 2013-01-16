/*!
  \class Project
  \ingroup DataStructures
  \brief A data class holding data for a project.
*/

#ifndef PROJECT_H
#define PROJECT_H

#include <QHash>
#include <QSet>
#include <QString>

#include "writingsystem.h"
#include "text.h"
#include "concordance.h"

class InterlinearItemType;
class TextBit;
class DatabaseAdapter;
class QUrl;
class QProgressDialog;

class Project : public QObject
{
    Q_OBJECT
public:
    Project();
    ~Project();

    enum OpenResult { Success, FileNotFound, XmlReadError };

    bool create(QString filename);
    bool readFromFile(QString filename);

    DatabaseAdapter* dbAdapter();

    QString projectPath() const;

    QSet<QString>* textPaths();

    bool save();
    void saveOpenTexts();
    void closeOpenTexts();

    void saveAndCloseText(Text *text);
    void closeText(Text *text);

    void deleteText(QString textName);

    Text* newText(const QString & name, const WritingSystem & ws, const QString &content = QString() , bool openText = true );
    Text* textFromFlexText(const QString & filePath, const WritingSystem & ws);
    Text* textFromFlexText(const QString & filePath);

    //! \brief Returns a list of text names, sorted alphabetically.
    QStringList textNames() const;

    QDir getTempDir() const;

    void removeTempDirectory();

    QHash<QString,Text*>* texts();

    OpenResult openText(const QString & name);

    QString filepathFromName(const QString & name) const;

    QString doDatabaseCleanup();
    int removeUnusedInterpretations();
    int removeUnusedGlosses();
    int removeUnusedTextForms();

    void setTextXmlFromDatabase();

    bool countItemsInTexts(const QString & filename, const QString & typeString);
    bool interpretationUsageReport(const QString & filename);
    bool outputInterpretationUsageReport(const QString & filename, const QStringList & instances );

    void applyXslTransformationToTexts(const QString & xslFile, const QHash<QString,QString> & parameters);
    bool applyXslTransformationToText(const QString & name, const QString & xslFile, const QHash<QString,QString> & parameters);

    Concordance* concordance();

public slots:

private:
    DatabaseAdapter *mDbAdapter;
    QString mDatabaseFilename;
    QString mDatabasePath;

    Concordance mConcordance;

    QString mProjectPath;

    void readTextPaths();
    bool maybeDelete(QDir tempDir);

    //! \brief Get all interpretation ids in use in this project
    QSet<qlonglong> getAllInterpretationIds();

    //! \brief Get all interpretation ids in use in the specified file path
    QSet<qlonglong> getSetOfNumbersFromTextQuery(const QString & filepath, const QString & queryString);

    //! \brief Get all gloss ids in use in this project
    QSet<qlonglong> getAllGlossIds();

    //! \brief Get all text form ids in use in this project
    QSet<qlonglong> getAllTextFormIds();

    QString tempDirName() const;

    //! \brief Paths of all texts in the temp directory
    QSet<QString> mTextPaths;

    //! \brief A hash containing all "opened" texts, keyed by name
    QHash<QString,Text*> mTexts;

    QList<qlonglong> getListOfNumbersFromXQuery(const QString & filepath, const QString & queryString);

    QStringList getStringListFromXQuery(const QString & filepath, const QString & queryString);

    QStringList getInterpretationUsage(const QString & filepath, const QString & encodedSettings);
};

#endif // PROJECT_H
