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

class InterlinearItemType;
class TextBit;
class DatabaseAdapter;

class Project : public QObject
{
    Q_OBJECT
public:
    Project();
    ~Project();

    bool create(QString filename);
    bool readFromFile(QString filename);

    DatabaseAdapter* dbAdapter();

    QString projectPath() const;

    QSet<QString>* textPaths();

    bool save();
    void saveOpenTexts();

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

    bool openText(const QString & name);

    QString filepathFromName(const QString & name) const;

    int removeUnusedGlossItems();

    QList<InterlinearItemType> interlinearItemsFromConfigurationFile(const QString & queryString) const;

    //! \brief Returns a list of types of interlinear text lines (InterlinearItemType objects) for the gloss tab.
    QList<InterlinearItemType> glossInterlinearLines() const;

    //! \brief Returns a list of phrasal gloss lines (InterlinearItemType objects) for the gloss tab.
    QList<InterlinearItemType> glossPhrasalGlossLines() const;

    //! \brief Returns a list of types of interlinear text lines (InterlinearItemType objects) for the analysis tab.
    QList<InterlinearItemType> analysisInterlinearLines() const;

    //! \brief Returns a list of phrasal gloss lines (InterlinearItemType objects) for the analysis tab.
    QList<InterlinearItemType> analysisPhrasalGlossLines() const;


public slots:

private:
    DatabaseAdapter *mDbAdapter;
    QString mDatabaseFilename;
    QString mDatabasePath;

    QList<InterlinearItemType> mGlossInterlinearLines;
    QList<InterlinearItemType> mGlossPhrasalGlossLines;
    QList<InterlinearItemType> mAnalysisInterlinearLines;
    QList<InterlinearItemType> mAnalysisPhrasalGlossLines;

    void parseConfigurationFile();

    QString mProjectPath;

    void readTextPaths();
    bool maybeDelete(QDir tempDir);

    //! \brief Get all interpretation ids in use in this project
    QSet<qlonglong> getAllInterpretationIds();

    //! \brief Get all interpretation ids in use in the specified file path
    QSet<qlonglong> getInterpretationIds(const QString & filepath);

    QString tempDirName() const;

    //! \brief Paths of all texts in the temp directory
    QSet<QString> mTextPaths;

    //! \brief A hash containing all "opened" texts, keyed by name
    QHash<QString,Text*> mTexts;

};

#endif // PROJECT_H
