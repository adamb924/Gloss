/*!
  \class Project
  \ingroup Data
  \brief A data class holding data for a project.
  \nosubgrouping
*/

#ifndef PROJECT_H
#define PROJECT_H

#include <QHash>
#include <QSet>
#include <QString>
#include <QPair>
#include <QDir>
#include <QRegularExpression>

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
    explicit Project(MainWindow *mainWindow);
    ~Project();

    //! \brief Result codes for opening texts
    enum OpenResult { Success, FileNotFound, XmlReadError };
    //! \brief Memory-modes; affects how the project opens and retains texts
    enum MemoryMode { OneAtATime, AccumulateSlowly, GreedyFast };

    /** @name Basic getters and setters
     * These methods provide read/write access to various objects
     */
    ///@{
public:
    //! \brief Returns the project meta language
    WritingSystem metaLanguage() const;

    //! \brief Returns the default language for glosses
    WritingSystem defaultGlossLanguage() const;

    //! \brief Returns the default language for text forms
    WritingSystem defaultTextFormLanguage() const;

    //! \brief Returns a hash of the opened texts (i.e., those in memory)
    QHash<QString,Text*>* openedTexts();

    //! \brief Returns a list of text names, sorted alphabetically.
    QStringList textNames() const;

    //! \brief Returns the number of texts in the project
    int textCount() const;

    //! \brief Returns the set of paths to texts in the project
    QSet<QString>* textPaths();

    //! \brief Returns the path of the project file
    QString projectPath() const;

    //! \brief Returns a pointer to the database adapter
    DatabaseAdapter* dbAdapter();

    //! \brief Returns a const pointer to the database adapter
    const DatabaseAdapter* dbAdapter() const;

    //! \brief Returns a pointer to the project concordance
    Concordance* concordance();

    //! \brief Returns a pointer to the main window
    const MainWindow* mainWindow() const;

    ///@}

    /** @name Project Input/Output
     * These methods handle input and output functions for the project
     */
    ///@{
public:
    //! \brief Creates the data structures in memory and on disk for a new project with filename \a filename
    bool create(QString filename);

    //! \brief Opens and reads the project at \a filename
    bool readFromFile(QString filename);

    //! \brief Saves all data to the project (.zip) file
    bool save();

    //! \brief Serializes the configuration.xml file
    void serializeConfigurationXml();

    //! \brief Returns true if this project has been changed, or if any of the texts has been changed
    bool isChanged() const;

private:
    //! \brief Reads the language settings from the configuration file
    void languageSettingsFromConfigurationFile();

    //! \brief Reads in the paths of all the texts in the project's temporary directory
    void readTextPaths();

    //! \brief Returns the path of the projects configuration file
    QString configurationXmlPath() const;

    //! \brief Parses the configuration file
    void parseConfigurationFile();

public slots:
    //! \brief Sets the project as having been modified
    void setChanged();

    ///@}

    /** @name Text Input/Output
     * These methods handle text input and output functions
     */
    ///@{

public:
    //! \brief Saves all texts that are open (i.e., in memory)
    void saveOpenTexts();

    //! \brief Closes all texts that are open (i.e., in memory) except those with names in \a except
    void closeOpenTexts( const QStringList & except );

    //! \brief Saves and closes \a text
    void saveAndCloseText(Text *text);

    //! \brief Closes \a text (without saving)
    void closeText(Text *text);

    //! \brief Deletes the text with name \a textName from the disk
    void deleteText(QString textName);

    //! \brief Creates a new text with name \a name, baseline writing system \a ws, and the initial content \a content. The lines of the text are determined by \a phraseDelimiter and \a paragraphDelimiter. Returns a pointer to the new text.
    Text* newText(const QString & name, const WritingSystem & ws, const QString &content, const QRegularExpression & phraseDelimiter = QRegularExpression("[\\n\\r]+"), const QRegularExpression & paragraphDelimiter = QRegularExpression("\\n\\r?\\n\\r?"));

    //! \brief Creates a new text by importing a Flex FlexText at \a filePath, with baseline writing system \a ws
    Text* importFlexText(const QString & filePath, const WritingSystem & ws);

    //! \brief Returns a pointer the named text, or 0 if there is an error. If the text is not already in memory it is loaded first.
    Text* text(const QString & name);

    //! \brief Opens a text with name \a name (i.e. \a name.flextext)
    OpenResult openText(const QString & name);

    //! \brief Returns the filepath in the temporary directory for the name \a name.
    QString filepathFromName(const QString & name) const;

    //! \brief Saves every Text in the project to XML format, with full strings included
    void setTextXmlFromDatabase();

    //! \brief Performs a search-and-replace operation on the baseline text of every open text
    void baselineSearchReplace( const TextBit & search , const TextBit & replace );

    //! \brief Renames the text names \a oldName to \a newName. If there is no text called \a oldName, nothing happens.
    void renameText( const QString & oldName, const QString & newName );

private:
    //! \brief Reads a text from \a filePath (using FlexTextReader)
    Text* textFromFlexText(const QString & filePath);

    //! \brief Opens the text at path \a path
    OpenResult openTextFromPath(const QString & path);

signals:
    //! \brief Emitted whenever a text is added to or deleted from the project
    void textsChanged();

    ///@}

    /** @name Temp Directory
     * These methods handle the project's temporary directory
     */
    ///@{
public:
    //! \brief Returns the project's directory in the temporary folder
    QDir getTempDir() const;

    //! \brief Removes the temporary directory
    void removeTempDirectory();

private:
    //! \brief Returns the temporary directory for the project
    QString tempDirName() const;

    ///@}

    /** @name Database cleanup methods
     * These methods pertain to database cleanup operations
     */
    ///@{
public:
    //! \brief Cleans up unused database entries.
    QString doDatabaseCleanup();

private:
    //! \brief Deletes interpretations that do not occur in the Text XML files. Returns the number removed.
    int removeUnusedInterpretations();

    //! \brief Deletes glosses that do not occur in the Text XML files.  Returns the number removed.
    int removeUnusedGlosses();

    //! \brief Deletes text forms that do not occur in the Text XML files. Returns the number removed.
    int removeUnusedTextForms();

    //! \brief Returns a set of interpretation ids in the texts by parsing their XML files
    QSet<qlonglong> getAllInterpretationIds();

    //! \brief Performs XQuery \a queryString on \a filepath. The \a queryString must return a set of numbers.
    QSet<qlonglong> getSetOfNumbersFromTextQuery(const QString & filepath, const QString & queryString);

    //! \brief Returns a set of gloss ids in use in the texts by parsing their XML files
    QSet<qlonglong> getAllGlossIds();

    //! \brief Returns a set of text form ids in use in the texts by parsing their XML files
    QSet<qlonglong> getAllTextFormIds();

    ///@}

    /** @name Media-related
     * These methods support playing the sounds for lines that have such data
     */
    ///@{
public:
    //! \brief Attempts to play the sound associated with \a textName for line \a lineNumber. This file reads the XML file, since a user may want to hear a line without opening the entire text.
    void playLine(const QString & textName, int lineNumber);

    //! \brief Returns a path to the filename, changing the path to the project's default media path, if the project is configured that way.
    QString mediaPath(const QString & path) const;

    //! \brief Returns the path to the media folder
    QString mediaFolder() const;

    //! \brief Returns whether the user wants the folder of the media files to be overridden (i.e., c:\wrong\path.wav can become c:\right\path.wav if the user specifies c:\right\ as the override path )
    bool overrideMediaPath() const;

public slots:
    //! \brief Sets whether the user wants the folder of the media files to be overridden (i.e., c:\wrong\path.wav can become c:\right\path.wav if the user specifies c:\right\ as the override path )
    void setOverrideMediaPath(bool value);

    //! \brief Sets the path to the media folder
    void setMediaFolder(const QString & folder);

private:
    //! \brief Prompts the user to change the media path, if the current path does not exist. @todo This should be in MainWindow
    void maybeUpdateMediaPath();

    ///@}

    /** @name Reports
     * These methods support the generation of reports about the project
     */
    ///@{
public:
    //! \brief Produces a CSV file (\a filename) with counts of either TextForms (if \a typeString is "txt") or Glosses (otherwise)
    bool countItemsInTexts(const QString & filename, const QString & typeString);

    //! \brief @todo This method is deprecated and should be removed
    bool interpretationUsageReport(const QString & filename);

    //! \brief @todo This method is deprecated and should be removed
    bool outputInterpretationUsageReport(const QString & filename, const QStringList & instances );

    //! \brief @todo This method is deprecated and should be removed
    static QStringList getInterpretationUsage(const QString & filepath, const QString & encodedSettings);

    ///@}

    /** @name Views
     * These methods support the various views of the project
     */
    ///@{
public:
    //! \brief Returns a const pointer to a list of the views
    const QList<View *> *views() const;

    //! \brief Returns a pointer to a list of the views
    QList<View*> *views();

    //! \brief Returns either the current full view or the current quick view, depending on the value of \a type
    const View * view(const View::Type type) const;

signals:
    //! \brief Emitted whenever the current full view changes
    void currentViewChanged(int index);

    //! \brief Emitted whenever the current quick view changes
    void currentQuickViewChanged(int index);

public slots:
    //! \brief Set the current full view to the \a index-th view
    void setView(int index);

    //! \brief Set the current quick view to the \a index-th view
    void setQuickView(int index);

    //! \brief Set the current full view, based on \a action's data property
    void setView(QAction * action);

    //! \brief Set the current quick view, based on \a action's data property
    void setQuickView(QAction * action);
    ///@}

    /** @name Memory mode
     * These methods support the memory mode for the project
     */
    ///@{
public:
    //! \brief Returns the current memory mode of the project
    Project::MemoryMode memoryMode() const;

    //! \brief Sets the current memory mode of the project
    void setMemoryMode( Project::MemoryMode mode );
private:
    //! \brief Loads every text of the project into memory
    void loadAllTextsIntoMemory();

    ///@}

    /** @name Lexical entry fields
     * These methods support project-level settings to do with lexical entries and glosses
     */
    ///@{
public:
    //! \brief Returns a pointer to the list of lexical entry citation fields
    QList<WritingSystem>* lexicalEntryCitationFormFields();

    //! \brief Returns a const pointer to the list of lexical entry citation fields
    const QList<WritingSystem>* lexicalEntryCitationFormFields() const;

    //! \brief Returns a pointer to the list of lexical entry gloss fields
    QList<WritingSystem>* lexicalEntryGlossFields();

    //! \brief Returns a const pointer to the list of lexical entry gloss fields
    const QList<WritingSystem>* lexicalEntryGlossFields() const;
private:
    //! \brief Reads lexical entry language preferences from the configuration file
    QList<WritingSystem> writingSystemListFromConfigurationFile(const QString & queryString) const;
    ///@}

    /** @name Annotations
     * These methods support project-level annotation settings
     */
    ///@{
public:
    //! \brief Returns a pointer to the list of annotation types in the project
    QList<AnnotationType>* annotationTypes();

    //! \brief Returns a const pointer to the list of annotation types in the project
    const QList<AnnotationType>* annotationTypes() const;

    //! \brief Returns the annotation type that has the label \a label, returning an empty AnnotationLabel if none exists
    AnnotationType annotationType(const QString & label) const;

private:
    //! \brief Reads the annotation types from the configuration file
    void annotationTypesFromConfigurationFile();
    ///@}

    /** @name XQuery
     * These methods support XQuery calls on texts at the project level
     */
    ///@{
public:
    //! \brief Performs XQuery \a queryString on the XML file at \a filepath, and returns a list of (qlonglong) integers. The query must return a list of integers.
    static QList<qlonglong> getListOfNumbersFromXQuery(const QString & filepath, const QString & queryString);

    //! \brief Performs XQuery \a queryString on the XML file at \a filepath, and returns a list of strings. The query must return a list of strings.
    static QStringList getStringListFromXQuery(const QString & filepath, const QString & queryString);

    //! \brief Performs XQuery \a queryString on the XML file at \a filepath, and returns a list of paired integers. The query must return a list of pairs of integers.
    static QList<LongLongPair> getPairedNumbersFromXQuery(const QString & filepath, const QString & queryString);

    ///@}

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
    bool mChanged;

    QString mDatabasePath;
    QDir mMediaPath;

    QList<AnnotationType> mAnnotationTypes;
    QList<WritingSystem> mLexicalEntryCitationForms;
    QList<WritingSystem> mLexicalEntryGlosses;
    WritingSystem mMetaLanguage;
    WritingSystem mDefaultTextFormLanguage;
    WritingSystem mDefaultGlossLanguage;
    QString mConfigurationXmlPath;

    QList<View*> mViews;

    //! \brief Paths of all texts in the temp directory
    QSet<QString> mTextPaths;

    //! \brief Returns a hash containing all "opened" texts, keyed by name
    QHash<QString,Text*> mTexts;

};

#endif // PROJECT_H
