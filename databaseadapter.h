/*!
  \class DatabaseAdapter
  \ingroup IO
  \brief A database adapter to provide an interface to the SQLite database, and also some occasional XQuery calls.
*/

#ifndef DATABASEADAPTER_H
#define DATABASEADAPTER_H

#include <QObject>
#include <QtSql>
#include <QHash>
#include <QList>

#include "textbit.h"
#include "interlinearitemtype.h"
#include "allomorph.h"
#include "annotationtype.h"
#include "syntactictype.h"
#include "partofspeech.h"

class WritingSystem;
class MorphologicalAnalysis;
class Concordance;

class DatabaseAdapter : public QObject
{
    Q_OBJECT
public:
    DatabaseAdapter(const QString & filename, QObject *parent = nullptr);
    ~DatabaseAdapter();

    //! \brief Returns a set with all interpretation ids
    QSet<qlonglong> interpretationIds() const;

    //! \brief Returns a set with all text form ids
    QSet<qlonglong> textFormIds() const;

    //! \brief Returns a set with all gloss ids
    QSet<qlonglong> glossIds() const;

    //! \brief Removes all interpretations with the given ids from the database
    int removeInterpretations( QSet<qlonglong> ids );

    //! \brief Removes all glosses with the given ids from the database
    int removeGlosses( QSet<qlonglong> ids );

    //! \brief Removes all text forms with the given ids from the database
    int removeTextForms( QSet<qlonglong> ids );

    //! \brief Returns a list of possible interpretations of the text from TextBit \a bit
    QList<qlonglong> candidateInterpretations(const TextBit & bit) const;

    //! \brief Returns a list of possible interpretations of the text from TextBit \a bit, along with associated text summaries.
    QHash<qlonglong,QString> candidateInterpretationWithSummaries(const TextBit & bit) const;

    //! \brief Returns a list of possible text forms for the given interpretation (\a interpretationId) and writing system (with _id \a writingSystemId)
    QHash<qlonglong,QString> interpretationTextForms(qlonglong interpretationId, qlonglong writingSystemId) const;

    TextBit interpretationTextForm(qlonglong interpretationId, qlonglong writingSystemId) const;

    //! \brief Returns a list of possible glosses for the given interpretation (\a interpretationId) and writing system (with _id \a writingSystemId)
    QHash<qlonglong,QString> interpretationGlosses(qlonglong interpretationId, qlonglong writingSystemId) const;

    TextBit interpretationGloss(qlonglong interpretationId, qlonglong writingSystemId) const;

    //! \brief Returns a list of possible interpretations of the given text and gloss forms
    QList<qlonglong> candidateInterpretations(const TextBitHash & textForms , const TextBitHash & glossForms );

    //! \brief Returns true if a the specified TextBit could belong to more than one Interpretation in the database, otherwise false.
    bool hasMultipleCandidateInterpretations(const TextBit & bit) const;

    //! \brief Creates a new interpretation and returns the database index of the Interpreation
    qlonglong newInterpretation();

    //! \brief Creates a new interpretation of the baseline TextBit \a bit and returns the database index of the Interpreation
    qlonglong newInterpretation( const TextBit & bit ) const;

    //! \brief Creates a new interpretation with the given text forms and glosses. Returns the _id of the interpretation
    qlonglong newInterpretation( TextBitHash & textForms , TextBitHash & glossForms ) const;

    //! \brief Creates a text form for the given interpretation, with the given writing system. Returns the _id of the text form.
    TextBit newTextForm(qlonglong interpretationId, qlonglong writingSystemId) const;

    //! \brief Creates a text form for the given interpretation and content in the \a bit. Or, if the a text form with the same form, writing system, and id already exists, the _id of that text form is returned and no new text form is created. Returns the _id of the text form.
    TextBit newTextForm(qlonglong interpretationId, const TextBit & bit) const;

    //! \brief Creates an empty gloss for the given interpretation, with the given writing system
    TextBit newGloss(qlonglong interpretationId, qlonglong writingSystemId) const;

    //! \brief Creates a gloss for the given interpretation and content in the \a bit. Or, if the a gloss with the same form, writing system, and id already exists, the _id of that gloss is returned and no new gloss is created. Returns the _id of the gloss.
    TextBit newGloss(qlonglong interpretationId, const TextBit & bit) const;

    //! \brief Returns a list of glosses for the given Interpretation _id (\a id), with one gloss per WritingSystem in the database, if one is available. This is not guaranteed to be a helpful return!
    TextBitHash guessInterpretationGlosses(qlonglong id) const;

    //! \brief Returns a list of text forms for the given Interpretation _id (\a id), with one gloss per WritingSystem in the database, if one is available. This is not guaranteed to be a helpful return!
    TextBitHash guessInterpretationTextForms(qlonglong id) const;

    //! \brief Returns a TextBit with the data from the \a row of Glosses, or an empty TextBit if none exists
    TextBit glossFromId(qlonglong glossId) const;

    //! \brief Returns a TextBit with the data from the \a row of TextForms, or an empty TextBit if none exists
    TextBit textFormFromId(qlonglong textFormId) const;

    qlonglong textFormId(const TextBit & bit, qlonglong interpretationId);
    qlonglong glossId(const TextBit & bit, qlonglong interpretationId);

    //! \brief Adds the writing system to the database
    void addWritingSystem(const QString & flexString, const QString & fontFamily, Qt::LayoutDirection layoutDirection);

    //! \brief Returns the WritingSystem specified by \a flexString, or an empty WritingSystem if none exists.
    WritingSystem writingSystem(const QString &  flexString) const;

    //! \brief Returns the WritingSystem specified by \a id, or n empty WritingSystem if none exists.
    WritingSystem writingSystem(qlonglong id) const;

    //! \brief Returns a list of all writing systems.
    QList<WritingSystem> writingSystems() const;

    //! \brief Returns true if a writing system with FlexString \a flexstring exists, otherwise false.
    bool writingSystemExists(const QString & flexstring) const;

    //! \brief Closes the database
    void close();

    //! \brief Returns the filename of the database (not the full path)
    QString dbFilename() const;

    //! \brief Returns true of an interpretation with _id \a id exists, otherwise false
    bool interpretationExists(qlonglong id) const;

    //! \brief Returns a string escaped for use in a SQL query.
    static QString sqlEscape(QString string)
    {
        return string.replace( "'" , "''");
    }

    //! \brief Returns a list of summaries of lexical candidates, indexed by lexical entry id, which are possible lexical entries for the given form.
    QList< QPair<qlonglong,QString> > getLexicalEntryCandidates( const TextBit & bit, const QString & morphologicalType, bool includeCitationForms = true ) const;

    //! \brief Returns a list of summaries of lexical candidates, indexed by lexical entry id, which are possible lexical entries for the given form.
    QSet<Allomorph::Type> getPossibleMorphologicalTypes( const TextBit & bit ) const;

    Allomorph::Type lexicalEntryMorphologicalType( qlonglong lexicalEntryId ) const;
    void setLexicalEntryMorphologicalType( qlonglong lexicalEntryId , Allomorph::Type type ) const;

    //! \brief Returns a list of summaries of lexical candidates, indexed by lexical entry id, treating \a bit as a search function.
    void searchLexicalEntries( const TextBit & bit , QHash<qlonglong,QString> * first , QHash<qlonglong,QString> * second ) const;

    //! \brief Returns a one-line summary of the lexical entry
    QString lexicalEntrySummary( qlonglong lexicalEntryId ) const;

    qlonglong addLexicalEntry( const QString & grammaticalInfo, Allomorph::Type type, const QList<TextBit> & glosses, const QList<TextBit> & citationForms, const QStringList & grammaticalTags ) const;
    qlonglong addAllomorph( const TextBit & bit , qlonglong lexicalEntryId ) const;

    bool hasMorphologicalAnalysis( qlonglong textFormId ) const;

    //! \brief Adds the specified morphological analysis to the database. Any existing morphological analysis associated with the text form is deleted. The
    void setMorphologicalAnalysis( qlonglong textFormId, const MorphologicalAnalysis * morphologicalAnalysis ) const;

    //! \brief Returns the morphological analysis associated with the given TextForm id. The analysis is empty if none is found in the database.
    MorphologicalAnalysis * morphologicalAnalysisFromTextFormId( qlonglong textFormId, Concordance * concordance ) const;

    void clearMorphologicalAnalysisIfInconsistent(qlonglong textFormId) const;

    //! \brief Returns true if the specified text form has a morphological analysis, otherwise false
    bool textFormHasMorphologicalAnalysis( qlonglong textFormId ) const;

    //! \brief Return an Allomorph object with data from the allomorph id indicated
    Allomorph *allomorphFromId( qlonglong allomorphId ) const;

    //! \brief Return the glosses associated with the given lexical entry
    TextBitHash lexicalItemGlosses(qlonglong lexicalEntryId) const;

    //! \brief Return the lexical entry citation forms for the given allomorph
    TextBitHash lexicalEntryCitationFormsForAllomorph(qlonglong allomorphId) const;

    //! \brief Return the lexical entry glosses for the given allomorph
    TextBitHash lexicalEntryGlossFormsForAllomorph(qlonglong allomorphId) const;

    //! \brief Return the lexical entry citation forms for the given lexical entry
    TextBitHash lexicalEntryCitationFormsForId(qlonglong lexicalEntryId) const;

    //! \brief Return the lexical entry glosses for the given lexical entry
    TextBitHash lexicalEntryGlossFormsForId(qlonglong lexicalEntryId) const;

    //! \brief Return a list of lexical entry forms
    QList<qlonglong> lexicalEntryIds() const;

    QStringList grammaticalTagsForAllomorph(qlonglong allomorphId) const;
    QStringList grammaticalTags(qlonglong lexicalEntryId) const;

    QList<TextBit> editTextFormsFromAllomorph(qlonglong allomorphId, const TextBit & oldForm, const TextBit & newForm) const;
    QStringList proposeEditTextFormsFromAllomorph(qlonglong allomorphId, const TextBit & oldForm, const TextBit & newForm) const;

    bool textIndicesShouldBeUpdated(const QStringList &textNames ) const;
    bool tableContainsAllTexts(const QString & tableName, const QSet<QString> & texts) const;
    void createTextIndices( const QSet<QString> * filePaths ) const;
    void createIndex( const QString & tableName, const QString & queryString, const QSet<QString> * filePaths ) const;
    void createTextFormIndex( const QSet<QString> * filePaths ) const;
    void createGlossIndex( const QSet<QString> * filePaths ) const;
    void createInterpretationIndex( const QSet<QString> * filePaths ) const;
    QSqlQuery searchIndexForTextForm( qlonglong id ) const;
    QSqlQuery searchIndexForGloss( qlonglong id ) const;
    QSqlQuery searchIndexForInterpretation( qlonglong id ) const;
    QSqlQuery searchIndexForLexicalEntry( qlonglong id ) const;
    QSqlQuery searchIndexForAllomorph( qlonglong id ) const;
    QSqlQuery searchIndexForText( const TextBit & bit ) const;
    QSqlQuery searchIndexForSubstring( const TextBit & bit ) const;
    QSet<qlonglong> lexicalEntryTextFormIds( qlonglong id ) const;
    QSet<qlonglong> allomorphTextFormIds( qlonglong id ) const;

    int removeUnusedMorphologicalAnalysisMembers() const;
    int removeUnusedAllomorphs() const;
    int removeUnusedLexicalEntries() const;

    QStringList availableGrammaticalTags() const;

    void setTagsForLexicalEntry( qlonglong lexicalEntryId, const QStringList & tags ) const;

    //! \brief Renames a lexical entry tag. If the new name is an existing tag, references to the old tag are replaced with references to the existing tag.
    void renameTag( const QString & oldName, const QString & newName ) const;

    void removeTag( const QString & tag ) const;

    QString guessGloss( const QString & hint , const WritingSystem & ws ) const;

    qlonglong lexicalEntryIdFromAllomorph(qlonglong allomorphId) const;

    QString guessLexicalEntryCitationForm( qlonglong lexicalEntryId , const WritingSystem & ws ) const;
    QStringList suggestLexicalEntryCitationForms( qlonglong lexicalEntryId , const WritingSystem & ws ) const;
    QString guessLexicalEntryGloss( qlonglong lexicalEntryId , const WritingSystem & ws ) const;
    QStringList suggestLexicalEntryGlosses( qlonglong lexicalEntryId , const WritingSystem & ws ) const;

    void loadWritingSystems();
    void loadSyntacticTypes();
    void loadWordPOS();
    void loadLexicalItemPOS();

    SyntacticType syntacticType(const QKeySequence & keySequence) const;
    SyntacticType syntacticType(const QString & abbreviation) const;

    QHash<QString, PartOfSpeech> lexicalItemPosByAbbreviation() const;
    QHash<QString, PartOfSpeech> wordPosByAbbreviation() const;

    bool multipleTextFormsAvailable(qlonglong interpretationId, const WritingSystem & ws) const;
    bool multipleGlossesAvailable(qlonglong interpretationId, const WritingSystem & ws) const;

    QList<WritingSystem> morphologicalAnalysisTextWritingSystems() const;
    QList<WritingSystem> morphologicalAnalysisGlossWritingSystems() const;

    int interpretationCountFromConcordance(qlonglong interpretationId) const;
    int textFormCountFromConcordance(qlonglong textFormId) const;
    int glossCountFromConcordance(qlonglong glossId) const;

    QList< QPair<TextBit, bool> > candidateMorphologicalSplits(const TextBit & textForm) const;
    QSet< QPair<TextBit, bool> > allSuffixPossibilities(const TextBit & textForm, bool testOther) const;
    QSet< QPair<TextBit, bool> > allPrefixPossibilities(const TextBit & textForm, bool testOther) const;
    QSet< QPair<TextBit, bool> > allTwoRootPossibilities(const TextBit & textForm) const;
    bool isStemAllomorph(const TextBit & textForm) const;

    bool hasDuplicateTextForms(qlonglong interpretationId, qlonglong writingSystemId) const;
    bool hasDuplicateGlosses(qlonglong interpretationId, qlonglong writingSystemId) const;

private:
    //! \brief Creates the tables; all tables are created with "create if not exists" -- so it's fine to call this in the constructor
    void createTables();

    QString mFilename;

    QList<WritingSystem> mWritingSystems;
    QHash<qlonglong, WritingSystem> mWritingSystemByRowId;
    QHash<QString, WritingSystem> mWritingSystemByFlexString;
    QHash<QKeySequence, SyntacticType> mSyntacticTypes;
    QHash<QString, SyntacticType> mSyntacticTypesByAbbreviation;

    QHash<QKeySequence, PartOfSpeech> mWordPOS;
    QHash<QString, PartOfSpeech> mWordPOSByAbbreviation;

    QHash<QKeySequence, PartOfSpeech> mLexicalItemPOS;
    QHash<QString, PartOfSpeech> mLexicalItemPOSByAbbreviation;

signals:

public slots:
    //! \brief Updates a gloss in the database, where the id of the gloss is the id field of \a bit
    void updateGloss( const TextBit & bit ) const;

    //! \brief Updates a text form in the database, where the id of the text form is the id field of \a bit
    void updateTextForm( const TextBit & bit ) const;

    void updateLexicalEntryCitationForm( const TextBit & bit ) const;
    void updateLexicalEntryGloss( const TextBit & bit ) const;
    void updateLexicalEntryType( qlonglong lexicalEntryId , const QString & allomorphType ) const;
    void updateLexicalEntryCitationForm( qlonglong lexicalEntryId, const TextBit & bit ) const;
    void updateLexicalEntryGloss( qlonglong lexicalEntryId, const TextBit & bit ) const;
};

#endif // DATABASEADAPTER_H
