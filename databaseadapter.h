/*!
  \class DatabaseAdapter
  \ingroup Data
  \brief A database adapter to provide an interface to the SQLite database.
*/

#ifndef DATABASEADAPTER_H
#define DATABASEADAPTER_H

#include <QObject>
#include <QtSql>
#include <QHash>
#include <QList>

#include "textbit.h"
#include "interlinearitemtype.h"

class WritingSystem;
class Allomorph;
class MorphologicalAnalysis;

class DatabaseAdapter : public QObject
{
    Q_OBJECT
public:
    DatabaseAdapter(const QString & filename, QObject *parent = 0);
    ~DatabaseAdapter();

    //! \brief Returns the database object
    QSqlDatabase* sqlDatabase();

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

    //! \brief Returns a list of possible glosses for the given interpretation (\a interpretationId) and writing system (with _id \a writingSystemId)
    QHash<qlonglong,QString> interpretationGlosses(qlonglong interpretationId, qlonglong writingSystemId) const;

    //! \brief Returns a list of possible interpretations of the given text and gloss forms
    QList<qlonglong> candidateInterpretations(const TextBitHash & textForms , const TextBitHash & glossForms );

    //! \brief Returns true if a the specified TextBit could belong to more than one Interpretation in the database, otherwise false.
    bool hasMultipleCandidateInterpretations(const TextBit & bit) const;

    //! \brief Creates a new interpretation of the baseline TextBit \a bit and returns the database index of the Interpreation
    qlonglong newInterpretation( const TextBit & bit );

    //! \brief Creates a new interpretation with the given text forms and glosses. Returns the _id of the interpretation
    qlonglong newInterpretation( TextBitHash & textForms , TextBitHash & glossForms );

    //! \brief Creates a text form for the given interpretation, with the given writing system. Returns the _id of the text form.
    qlonglong newTextForm(qlonglong interpretationId, qlonglong writingSystemId);

    //! \brief Creates a text form for the given interpretation and content in the \a bit. Or, if the a text form with the same form, writing system, and id already exists, the _id of that text form is returned and no new text form is created. Returns the _id of the text form.
    qlonglong newTextForm(qlonglong interpretationId, const TextBit & bit);

    //! \brief Creates an empty gloss for the given interpretation, with the given writing system
    qlonglong newGloss(qlonglong interpretationId, qlonglong writingSystemId);

    //! \brief Creates a gloss for the given interpretation and content in the \a bit. Or, if the a gloss with the same form, writing system, and id already exists, the _id of that gloss is returned and no new gloss is created. Returns the _id of the gloss.
    qlonglong newGloss(qlonglong interpretationId, const TextBit & bit);

    //! \brief Returns a list of glosses for the given Interpretation _id (\a id), with one gloss per WritingSystem in the database, if one is available. This is not guaranteed to be a helpful return!
    TextBitHash guessInterpretationGlosses(qlonglong id) const;

    //! \brief Returns a list of text forms for the given Interpretation _id (\a id), with one gloss per WritingSystem in the database, if one is available. This is not guaranteed to be a helpful return!
    TextBitHash guessInterpretationTextForms(qlonglong id) const;

    //! \brief Returns a TextBit with the data from the \a row of Glosses, or an empty TextBit if none exists
    TextBit glossFromId(qlonglong id) const;

    //! \brief Returns a TextBit with the data from the \a row of TextForms, or an empty TextBit if none exists
    TextBit textFormFromId(qlonglong id) const;

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

    //! \brief Creates the tables for the first time
    void createTables();

    //! \brief Returns true of an interpretation with _id \a id exists, otherwise false
    bool interpretationExists(qlonglong id) const;

    //! \brief Returns a string escaped for use in a SQL query.
    static QString sqlEscape(QString string)
    {
        return string.replace( "'" , "''");
    }

    void parseConfigurationFile(const QString & filename);

    //! \brief Returns a list of types of interlinear text lines (InterlinearItemType objects) for the gloss tab.
    QList<InterlinearItemType> glossInterlinearLines() const;

    //! \brief Returns a list of phrasal gloss lines (InterlinearItemType objects) for the gloss tab.
    QList<InterlinearItemType> glossPhrasalGlossLines() const;

    //! \brief Returns a list of types of interlinear text lines (InterlinearItemType objects) for the analysis tab.
    QList<InterlinearItemType> analysisInterlinearLines() const;

    //! \brief Returns a list of phrasal gloss lines (InterlinearItemType objects) for the analysis tab.
    QList<InterlinearItemType> analysisPhrasalGlossLines() const;

    //! \brief Returns the meta analysis language
    WritingSystem metaLanguage() const;

    QList<WritingSystem> lexicalEntryCitationForms() const;
    QList<WritingSystem> lexicalEntryGlosses() const;

    QHash<qlonglong,QString> getLexicalEntryCandidates( const TextBit & bit );

    qlonglong addLexicalEntry( const QString & grammaticalInfo, const QList<TextBit> & glosses, const QList<TextBit> & citationForms );
    qlonglong addAllomorph( const TextBit & bit , qlonglong lexicalEntryId );

    //! \brief Adds the specified morphological analysis to the database. Any existing morphological analysis associated with the text form is deleted. The
    void setMorphologicalAnalysis( qlonglong textFormId, const MorphologicalAnalysis & morphologicalAnalysis );

    //! \brief Returns the morphological analysis associated with the given TextForm id. The analysis is empty if none is found in the database.
    MorphologicalAnalysis morphologicalAnalysisFromTextFormId( qlonglong textFormId );

    //! \brief Return an Allomorph object with data from the allomorph id indicated
    Allomorph allomorphFromId( qlonglong allomorphId );

    //! \brief Return the glosses associated with the given lexical entry
    TextBitHash lexicalItemGlosses(qlonglong lexicalEntryId) const;

    //! \brief Return the lexical entry citation forms for the given allomorph
    TextBitHash lexicalEntryCitationFormsForAllomorph(qlonglong id) const;

    //! \brief Return the lexical entry glosses for the given allomorph
    TextBitHash lexicalEntryGlossFormsForAllomorph(qlonglong id) const;

private:
    QString mFilename;

    void loadWritingSystems();
    QList<WritingSystem> mWritingSystems;
    QHash<qlonglong, WritingSystem> mWritingSystemByRowId;
    QHash<QString, WritingSystem> mWritingSystemByFlexString;

    QList<InterlinearItemType> interlinearItemsFromConfigurationFile(const QString & queryString) const;
    QList<WritingSystem> writingSystemListFromConfigurationFile(const QString & queryString) const;
    void metalanguageFromConfigurationFile();

    QList<InterlinearItemType> mGlossInterlinearLines;
    QList<InterlinearItemType> mGlossPhrasalGlossLines;
    QList<InterlinearItemType> mAnalysisInterlinearLines;
    QList<InterlinearItemType> mAnalysisPhrasalGlossLines;

    QList<WritingSystem> mLexicalEntryCitationForms;
    QList<WritingSystem> mLexicalEntryGlosses;

    WritingSystem mMetaLanguage;

    QString mConfigurationXmlPath;

signals:

public slots:
    //! \brief Updates a gloss in the database, where the id of the gloss is the id field of \a bit
    void updateInterpretationGloss( const TextBit & bit );

    //! \brief Updates a text form in the database, where the id of the text form is the id field of \a bit
    void updateInterpretationTextForm( const TextBit & bit );

};

#endif // DATABASEADAPTER_H
