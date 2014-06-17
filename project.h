/*!
  \class Project
  \ingroup Data
  \brief A data class holding data for a project.
*/

#ifndef PROJECT_H
#define PROJECT_H

#include <QHash>
#include <QSet>
#include <QString>
#include <QPair>
#include <QDir>

#include "writingsystem.h"
#include "text.h"
#include "concordance.h"
#include "view.h"

class InterlinearItemType;
class TextBit;
class DatabaseAdapter;
class QUrl;
class QProgressDialog;
class MainWindow;
class QAction;
class AnnotationType;

typedef QPair<qlonglong,qlonglong> LongLongPair;

class Project : public QObject
{
    Q_OBJECT
public:
    Project(MainWindow *mainWindow);
    ~Project();

    enum OpenResult { Success, FileNotFound, XmlReadError };
    enum MemoryMode { OneAtATime, AccumulateSlowly, GreedyFast };

    bool create(QString filename);
    bool readFromFile(QString filename);

    DatabaseAdapter* dbAdapter();
    const DatabaseAdapter* dbAdapter() const;

    QString projectPath() const;

    QSet<QString>* textPaths();

    bool save();
    void saveOpenTexts();
    void closeOpenTexts( const QStringList & except );

    void saveAndCloseText(Text *text);
    void closeText(Text *text);

    void deleteText(QString textName);

    Text* newText(const QString & name, const WritingSystem & ws, const QString &content = QString() );
    Text* importFlexText(const QString & filePath, const WritingSystem & ws);
    Text* textFromFlexText(const QString & filePath);

    //! \brief Returns a list of text names, sorted alphabetically.
    QStringList textNames() const;

    QDir getTempDir() const;

    void removeTempDirectory();

    QHash<QString,Text*>* openedTexts();

    //! \brief Returns a pointer the named text, or 0 if there is an error. If the text is not already in memory it is loaded first.
    Text* text(const QString & name);

    OpenResult openText(const QString & name);
    OpenResult openTextFromPath(const QString & path);

    QString filepathFromName(const QString & name) const;
    QString nameFromFilepath(const QString & path) const;

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
    const MainWindow* mainWindow() const;

    void playLine(const QString & textName, int lineNumber);

    const QList<View *> *views() const;
    QList<View*> *views();


    const View * view(const View::Type type) const;

    Project::MemoryMode memoryMode() const;
    void setMemoryMode( Project::MemoryMode mode );

    void baselineSearchReplace( const TextBit & search , const TextBit & replace );

    //! \brief Returns a path to the filename, changing the path to the project's default media path, if the project is configured that way.
    QString mediaPath(const QString & path) const;

    QString mediaFolder() const;

    QList<WritingSystem>* lexicalEntryCitationFormFields();
    const QList<WritingSystem>* lexicalEntryCitationFormFields() const;
    QList<WritingSystem>* lexicalEntryGlossFields();
    const QList<WritingSystem>* lexicalEntryGlossFields() const;

    QList<AnnotationType>* annotationTypes();
    const QList<AnnotationType>* annotationTypes() const;
    AnnotationType annotationType(const QString & label) const;

    //! \brief Returns the meta analysis language
    WritingSystem metaLanguage() const;
    WritingSystem defaultGlossLanguage() const;
    WritingSystem defaultTextFormLanguage() const;

    void serializeConfigurationXml();

public slots:
    void setInterlinearView(QAction * action);
    void setQuickView(QAction * action);
    void setOverrideMediaPath(bool value);

    void setMetaLanguage(const WritingSystem & ws);
    void setDefaultGlossLanguage(const WritingSystem & ws);
    void setDefaultTextFormLanguage(const WritingSystem & ws);
    void setMediaFolder(const QString & folder);

private:
    QString mDatabasePath;
    QDir mMediaPath;

    QList<AnnotationType> mAnnotationTypes;
    QList<WritingSystem> mLexicalEntryCitationForms;
    QList<WritingSystem> mLexicalEntryGlosses;
    WritingSystem mMetaLanguage;
    WritingSystem mDefaultTextFormLanguage;
    WritingSystem mDefaultGlossLanguage;
    QString mConfigurationXmlPath;

    void parseConfigurationFile();
    QList<WritingSystem> writingSystemListFromConfigurationFile(const QString & queryString) const;
    void languageSettingsFromConfigurationFile();
    void annotationTypesFromConfigurationFile();

    void maybeUpdateMediaPath();

    QList<View*> mViews;

    void readTextPaths();

    void loadAllTextsIntoMemory();

    //! \brief Returns a set of interpretation ids in use in this project
    QSet<qlonglong> getAllInterpretationIds();

    //! \brief Returns a set of interpretation ids in use in the specified file path
    QSet<qlonglong> getSetOfNumbersFromTextQuery(const QString & filepath, const QString & queryString);

    //! \brief Returns a set of gloss ids in use in this project
    QSet<qlonglong> getAllGlossIds();

    //! \brief Returns a set of text form ids in use in this project
    QSet<qlonglong> getAllTextFormIds();

    //! \brief Returns the temporary directory for the project
    QString tempDirName() const;

    //! \brief Paths of all texts in the temp directory
    QSet<QString> mTextPaths;

    //! \brief Returns a hash containing all "opened" texts, keyed by name
    QHash<QString,Text*> mTexts;

public:
    static QList<qlonglong> getListOfNumbersFromXQuery(const QString & filepath, const QString & queryString);

    static QStringList getStringListFromXQuery(const QString & filepath, const QString & queryString);

    static QList<LongLongPair> getPairedNumbersFromXQuery(const QString & filepath, const QString & queryString);

    static QStringList getInterpretationUsage(const QString & filepath, const QString & encodedSettings);
    bool overrideMediaPath() const;

private:
    MainWindow *mMainWindow;
    QString mDatabaseFilename;
    DatabaseAdapter *mDbAdapter;
    bool mOverrideMediaPath;
    Project::MemoryMode mMemoryMode;
    View * mCurrentInterlinearView;
    View * mCurrentQuickView;

    QString mProjectPath;
    Concordance mConcordance;
};

#endif // PROJECT_H
