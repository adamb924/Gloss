/*!
  \class Text
  \ingroup Data
  \brief A data class holding data for a text.
  \nosubgrouping
*/

#ifndef TEXT_H
#define TEXT_H

#include <QObject>
#include <QString>
#include <QList>
#include <QUrl>
#include <QRegularExpression>

#include "textbit.h"
#include "flextextreader.h"

class WritingSystem;
class Project;
class QXmlStreamWriter;
class QDir;
class DatabaseAdapter;
class Sound;
class SyntacticAnalysis;
class GlossItem;
class Phrase;
class TextBit;
class Paragraph;

class Text : public QObject
{
    Q_OBJECT

    friend class FlexTextReader;
    friend class FlexTextWriter;
    friend class FlexTextImporter;

public:
    //! \brief Result codes for merging a translation from another FlexText file
    enum MergeTranslationResult { Success, MergeStuckOldFileDeleted, MergeStuckOldFileStillThere, XslTranslationError };

    //! \brief Result codes for merging time stamps in from an EAF file
    enum MergeEafResult { MergeEafSuccess, MergeEafFailure, MergeEafWrongNumberOfAnnotations };

    //! \brief Result codes for reading a FlexText
    enum FlexTextReadResult { FlexTextReadSuccess, FlexTextReadBaselineNotFound, FlexTextReadXmlReadError, FlexTextReadNoAttempt };

    //! \brief Creates a text with baseline WritingSystem \a ws and name \a name
    Text(const WritingSystem & ws, const QString & name, Project *project);

    //! \brief Creates a text from the FlexText at \a filePath
    Text(const QString & filePath, Project *project);
    ~Text();

    /** @name Basic getters and setters
     * These methods provide read/write access to various objects
     */
    ///@{
public:
    //! \brief Returns the name of the text
    QString name() const;

    //! \brief Sets the name of the text
    void setName(const QString & name);

    //! \brief Returns the export filename of the text, if one has been set. Otherwise returns the text name appended with ".flextext"
    QString exportFilename() const;

    //! \brief Sets the export filename of the text. This is the filename that will be used for exporting the text.
    void setExportFilename(const QString & filename);

    //! \brief Returns the comment of the text
    QString comment() const;

    //! \brief Sets the comment of the text
    void setComment(const QString & comment);

    //! \brief Returns true if the text has been changed since it was opened.
    bool isChanged() const;

    //! \brief Returns the baseline WritingSystem of the text
    WritingSystem baselineWritingSystem() const;

    //! \brief Returns a const pointer the Project that the text is part of
    const Project * project() const;

    //! \brief Returns a pointer to the list of Paragraphs in the text
    QList<Paragraph*>* paragraphs();

    //! \brief Returns a const pointer to the list of Paragraphs in the text
    const QList<Paragraph *> *paragraphs() const;

    //! \brief Returns true if the Text is valid, otherwise false. A Text can be invalid if there has been some error, for instance.
    bool isValid() const;

public slots:
    //! \brief Marks the text as having changed
    void markAsChanged();
    ///@}

    /** @name Text input/output
    * Methods for reading or writing the text on the disk
    */
    ///@{
public:
    //! \brief Writes the text to the specified \a path using FlexTextWriter. \a verboseOutput and \a glossNamespace are passed to FlexTextWriter
    void writeTextTo(const QString & path, bool verboseOutput, bool glossNamespace);

    //! \brief Writes the text to its file path using FlexTextWriter, if either the text has been changed, or \a saveAnyway is true. \a verboseOutput and \a glossNamespace are passed to FlexTextWriter
    void saveText(bool verboseOutput, bool glossNamespace, bool saveAnyway);

    //! \brief Reads the text from its path using FlexTextReader
    FlexTextReader::Result readResult() const;
    ///@}

    /** @name Paragraph operations
    * Methods for performing actions on the paragraphs of the text
    */
    ///@{

    //! \brief Returns a pointer to the paragraph containing \a phrase, or zero if no paragraph in the text contains \a phrase.
    Paragraph * paragraphForPhrase(Phrase *phrase);

    //! \brief Returns the (0-indexed) position of this phrase, in this text. If the phrase does not exist in the text, returns -1.
    int lineNumberForPhrase(Phrase *phrase) const;

    //! \brief Returns the (0-indexed) position of this phrase, in this text. If the phrase does not exist in the text, returns -1.
    int lineNumberForGlossItem(const GlossItem *item) const;

    //! \brief Removes the paragraph division at position \a paragraphIndex
    void removeParagraphDivision(int paragraphIndex);

    //! \brief Adds a paragraph division after position \a paragraphIndex, with \a phraseIndex as the first phrase of the new paragraph
    void addParagraphDivision(int paragraphIndex, int phraseIndex);

public slots:
    //! \brief Removes the paragraph division \a paragraph
    void removeParagraphDivision(Paragraph * paragraph);

    //! \brief Adds a paragraph division starting at line \a lineNumber
    void addParagraphDivision(int lineNumber);

    ///@}

    /** @name Phrase operations
    * Methods for performing actions on the phrases of the text
    */
    ///@{
public slots:
    //! \brief Returns a pointer to the phrase for line \a lineNumber.
    Phrase * phraseAtLine(int lineNumber);

    //! \brief Returns a const pointer to the phrase for line \a lineNumber.
    const Phrase * phraseAtLine(int lineNumber) const;

    //! \brief Returns the number of phrases in a text.
    int phraseCount() const;

    //! \brief Removes phrase \a index
    void removePhrase(int lineNumber );

    //! \brief Creates a new Phrase, splitting the Phrase that contains \a glossItem into two, with \a glossItem beginning the new phrase.
    void newPhraseStartingHere(GlossItem * glossItem);

    //! \brief Removes the Phrase containing \a glossItem, placing those GlossItem objects at the end of the preceding phrase.
    void noNewPhraseStartingHere(GlossItem * glossItem);

private slots:
    //! \brief If \a thisPhrase is part of the text, emits phrasalGlossChanged()
    void registerPhrasalGlossChange(Phrase *thisPhrase, const TextBit & bit);

    //! \brief Removes \a phrase from the text
    void removePhrase( Phrase * phrase );

signals:
    //! \brief Indicates that the \a index -th Phrase needs to be redrawn
    void phraseRefreshNeeded( int index );

    //! \brief Indicates that the phrasal gloss (for the writing system in \a bit) \a index -th Phrase has changed to \a bit.
    void phrasalGlossChanged( int index, const TextBit & bit);
    ///@}

    /** @name Gloss item operations
    * Methods for performing actions on the GlossItems of the text
    */
    ///@{
public:
    //! \brief Sets the baseline text for phrase \a i to \a text (creating GlossItems, etc.)
    void setBaselineTextForPhrase( int i, const QString & text );

    //! \brief Searches for \a glossItem and sets \a line to the phrase index and \a position to the position of the GlossItem within the phrase. All values are 0-indexed. If \a glossItem is not found, \a line and \a position are both set to -1.
    void findGlossItemLocation(const GlossItem *glossItem, int &paragraph, int & phrase, int & position) const;

    //! \brief Initializes the text with the text \a content. The text is split into paragraphs using \a paragraphDelimiter, and then into phrases using \a phraseDelimiter.
    void initializeTextFromString(const QString &content, const QRegularExpression & phraseDelimiter = QRegularExpression("[\\n\\r]+"), const QRegularExpression & paragraphDelimiter = QRegularExpression("\\n\\r?\\n\\r?"));

public slots:
    //! \brief Changes all GlossItem objects following \a glossItem to match the interpretation of \a glossItem
    void setFollowingInterpretations( GlossItem *glossItem );

    //! \brief Changes the baseline text of all GlossItem objects with the same id as \a glossItem, so that their baseline text matches that of \a glossItem
    void replaceFollowing(GlossItem *glossItem, const QString & searchFor );

    //! \brief Perform a search-and-replace on all GlossItem objects in the text, replacing \a search with \a replace
    void baselineSearchReplace( const TextBit & search , const TextBit & replace );

    //! \brief Sets the text forms with WritingSystem \a w of all GlossItem objects following \a glossItem to match \a glossItem for that WritingSystem
    void matchFollowingTextForms(GlossItem *glossItem, const WritingSystem & ws );

    //! \brief Sets the glosses with WritingSystem \a w of all GlossItem objects following \a glossItem to match \a glossItem for that WritingSystem
    void matchFollowingGlosses(GlossItem *glossItem, const WritingSystem & ws );

private:
    //! \brief Removes all gloss items from the text
    void clearGlossItems();

    //! \brief Changes \a phrase so that it contains the text in \a line (creating GlossItems, etc.)
    /// @todo Why is this here and not in Phrase?
    void setLineOfGlossItems(Phrase *phrase , const QString & line );

signals:
    //! \brief Emitted whenever the GlossItem objects of the text are changed
    void glossItemsChanged();

    ///@}

    /** @name Merge operations
    * Methods for merging in data from EAF files or other FlexTexts
    */
    ///@{
public:
    //! \brief Merges the phrase-level glosses in the FlexText \a filename using WritingSystem \a ws
    /// @todo This should almost certainly not use Xsltproc. XQuery would be better.
    Text::MergeTranslationResult mergeTranslation(const QString & filename, const WritingSystem & ws );

    //! \brief Merges the audio annotations from the EAF file specified by \a filename
    Text::MergeEafResult mergeEaf(const QString & filename );
    ///@}

    /** @name Sound methods
    * Methods for playing sounds associated with a text
    */
    ///@{
public:
    //! \brief Sets the sound associated with the file to \a filename
    void setSound(const QUrl & filename);

public slots:
    //! \brief Plays the sound for the given 0-indexed line number, or returns an error.
    bool playSoundForLine( int lineNumber );
    ///@}

    /** @name Syntactic analysis methods
    * Methods to do with the syntactic analysis of a text
    */
    ///@{
public:
    //! \brief Returns all of the SyntacticAnalysis objects associated with the project, keyed to the name of each analysis.
    QHash<QString, SyntacticAnalysis*>* syntacticAnalyses();
    ///@}

    /** @name Miscellaneous methods
    * Methods that don't fit anywhere else. :-)
    */
    ///@{
public:
    //! \brief Returns the name of a text, for a given path. This just uses QFileInfo::baseName()
    static QString textNameFromPath(const QString & path);

private slots:
    //! \brief Emits guiRefreshRequest, if \a phrase is part of this text
    void requestGuiRefresh( Phrase * phrase );

signals:
    //! \brief Requests that the GUI for \a phrase be redrawn
    void guiRefreshRequest();

    //! \brief Emitted whenever the GlossItem annotations change
    void annotationsChanged();
    ///@}

private:
    Sound *mSound;
    FlexTextReader::Result mReadResult;
    bool mValid;
    bool mChanged;

    QString mName;
    QString mExportFilename;
    WritingSystem mBaselineWritingSystem;
    Project *mProject;
    DatabaseAdapter *mDbAdapter;

    QHash<QString, SyntacticAnalysis*> mSyntacticAnalyses;

    QString mComment;
    QUrl mAudioFileURL;

    QList<Phrase*> mPhrases;
    QList<Paragraph*> mParagraphs;
};

#endif // TEXT_H
