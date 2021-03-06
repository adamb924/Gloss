/*!
  \class GlossItem
  \ingroup Data
  \brief A data class representing a gloss item in an interlinear text. This consists of an id (the _id field of the Interpretations SQL table), hashes of text and gloss lines, and a pointer to the TextBit of the baseline text.

  Each GlossItem is associated with a row from the Interpretations SQL table (mId corresponds _id). The other data values are not necessarily predictable from this information, however, since there can be many-to-one relationships between text forms or glosses and interpretations, however.

  GlossItem objects representing punctuation are the same as any other GlossItem. It is the responsibility of the GUI, and input/output code, to display and record these elements appropriately. The GlossItem::isPunctuation() returns true if the baseline text of the object is nothing but punctuation.
*/

#ifndef GLOSSITEM_H
#define GLOSSITEM_H

#include <QObject>
#include <QHash>
#include <QList>

#include "textbit.h"
#include "allomorph.h"
#include "morphologicalanalysis.h"
#include "annotation.h"
#include "partofspeech.h"

class TextBit;
class Project;
class DatabaseAdapter;
class Concordance;
class Focus;

class GlossItem : public QObject
{
    Q_OBJECT
public:
    enum CandidateNumber { SingleOption, MultipleOption };
    enum ApprovalStatus { Approved, Unapproved };

    //! \brief Construct a GlossItem that is empty except for the baseline TextBit.
    GlossItem(const TextBit & baselineBit, Project *project, QObject *parent = nullptr);

    //! \brief Construct a GlossItem with the given WritingSystem, and gloss and text forms. Any id values are ignored. The interpretation id is guessed, or a new one is created if there are no possibilities. This constructor is for importing Flex Flextext files.
    GlossItem(const WritingSystem & ws, const TextBitHash & textForms, const TextBitHash & glossForms, Project *project, QObject *parent = nullptr);

    //! \brief Construct a GlossItem with the given WritingSystem, interpretation id, and the ids of the given gloss and text forms. This constructor is for reading Flextext files that Gloss created.
    GlossItem(const WritingSystem & ws, const QSet<qlonglong> & textForms, const QSet<qlonglong> & glossForms, qlonglong interpretationId, Project *project, QObject *parent = nullptr);

    ~GlossItem();

    //! \brief Set the baseline text for the gloss item, guessing the proper interpretation
    void resetBaselineText( const TextBit & baselineBit );

    //! \brief Sets the id of this GlossItem (corresponding to the _id row of the Interpretations SQL table), and sets the data for the GlossItem accordingly. If \a takeFormsFromDatabase is true, the GlossItem is filled with values from the database.
    void setInterpretation(qlonglong id );

    //! \brief Returns the id of the GlossItem (corresponding to the _id row of the Interpretations SQL table)
    qlonglong id() const;

    //! \brief Returns the PartOfSpeech of the GlossItem
    PartOfSpeech pos() const;

    //! \brief Returns the TextBit of the baseline text.
    TextBit baselineText() const;

    //! \brief Returns a pointer to a TextBitHash with the text forms. See textForm() for an alternate interface to this data structure.
    const TextBitHash* textForms() const;

    //! \brief Returns a pointer to a TextBitHash with the glosses. See gloss() for an alternate interface to this data structure.
    const TextBitHash* glosses() const;

    //! \brief Returns the text form for the given writing system. If one is not available in the has, one is returned from the database. If there is none in the database, a new one is created.
    TextBit textForm(const WritingSystem & ws);

    //! \brief Returns the gloss for the given writing system. If one is not available in the has, one is returned from the database. If there is none in the database, a new one is created.
    TextBit gloss(const WritingSystem & ws);

    //! \brief Set the candidate status of the GlossItem to \a status.
    void setCandidateNumber(CandidateNumber status);

    //! \brief Set the approval status of the GlossItem to \a status.
    void setApprovalStatus(ApprovalStatus status);

    //! \brief Returns the approval status of the GlossItem
    ApprovalStatus approvalStatus() const;

    //! \brief Returns the candidate status of the GlossItem
    CandidateNumber candidateNumber() const;

    //! \brief Returns the WritingSystem of the GlossItem's baseline text
    WritingSystem baselineWritingSystem() const;

    //! \brief Returns the morphological analysis for the given writing system, or an empty one
    MorphologicalAnalysis * morphologicalAnalysis(const WritingSystem & ws);

    //! \brief Returns the morphological analysis for the given writing system, or an empty one
    const MorphologicalAnalysis * morphologicalAnalysis(const WritingSystem & ws) const;

    //! \brief Returns a list of languages for which there are morphological analyses
    QList<WritingSystem> nonEmptyMorphologicalAnalysisLanguages() const;

    //! \brief Sets the morphological analysis for the given writing system
    void setMorphologicalAnalysis(MorphologicalAnalysis * analysis );

    //! \brief Returns true if this gloss item matches the specified focus, otherwise false
    bool matchesFocus( const Focus & focus ) const;

    //! \brief Returns the annotation for the given \a key, or an empty TextBit if none exists
    Annotation getAnnotation( const QString & key ) const;

    //! \brief Sets the annotation for the given \a key to \a annotation
    void setAnnotation( const QString & key, const Annotation & annotation );

    //! \brief Returns all annotations associated with the GlossItem
    QHashIterator<QString, Annotation> annotations() const;

    //! \brief Returns true if the GlossItem has at least one annotation, otherwise returns false
    bool hasAnnotations() const;

    //! \brief Returns true if the GlossItem an annotation for the specified \a key, otherwise returns false
    bool hasAnnotation( const QString & key ) const;

    //! \brief Returns a pointer to the project's Concordance object
    Concordance * concordance();

    //! \brief Returns a pointer to the project's Project object
    Project * project();

    //! \brief Connects various signals of the object to the project's Concordance object
    void connectToConcordance();

    //! \brief Loads all morphological analyses in the database into the object's data structure. This method will change all of the GUIDs of the allomorphs.
    void loadMorphologicalAnalysesFromDatabase();

    //! \brief Returns true if the object represents punctuation; otherwise false.
    bool isPunctuation() const;

    bool multipleTextFormsAvailable( const WritingSystem & ws ) const;
    bool multipleGlossesAvailable( const WritingSystem & ws ) const;

    void setMultipleTextFormsAvailable(const WritingSystem & ws, bool multiple);
    void setMultipleGlossesAvailable( const WritingSystem & ws, bool multiple );

    void emitTextFormChangedSignal(const TextBit & textForm);

signals:
    //! \brief Emitted when the candidate status of the GlossItem changes
    void candidateNumberChanged( GlossItem::CandidateNumber mCandidateNumber, qlonglong interpretationId );

    void textFormNumberChanged( bool multipleAvailable, qlonglong interpretationId, const WritingSystem & ws);

    void glossNumberChanged( bool multipleAvailable, qlonglong interpretationId, const WritingSystem & ws);

    //! \brief Emitted when the approval status of the GlossItem changes
    void approvalStatusChanged(GlossItem::ApprovalStatus status);

    //! \brief Emitted whenever the text form or glosses have changed. This is currently used only by WordDisplayWidget
    void fieldsChanged();

    //! \brief Emitted whenever a text form changes. This is currently used by Concordance
    void textFormChanged(const TextBit & textForm);

    //! \brief Emitted whenever a gloss changes
    void glossChanged(const TextBit & gloss);

    //! \brief Emitted whenever the baseline text changes
    void baselineTextChanged(const TextBit & textForm);

    //! \brief Emitted when the morphological analysis for \a textFormId changes
    void morphologicalAnalysisChanged(const GlossItem * originator, const MorphologicalAnalysis * analysis);

    void annotationsChanged();

public slots:
    //! \brief Toggles the approval status of the GlossItem
    void toggleApproval();

    //! \brief Sets a gloss to the value indicated. The gloss that is updated is the one that matches the WritingSystem of \a gloss.
    void setGloss(const TextBit & gloss);

    //! \brief Changes the text of the gloss, if the id matches
    void setGlossText(const TextBit & gloss);

    //! \brief Sets a text form to the value indicated. The text form that is updated is the one that matches the WritingSystem of \a textForm.
    void setTextForm(const TextBit & textForm);

    //! \brief Changes the text of the text form, if the id matches
    void setTextFormText(const TextBit & textForm);

    //! \brief Changes the part of speech
    void setPos(const PartOfSpeech & pos);

private:
    //! \brief Attempt to set the (interpretation) id of \a bit by querying the database for interpretations compatible with the text forms and gloss forms, or if there are none, than for those compatible with the baseline bit. If no compatible interpretation is found, a new interpretation is created.
    void guessInterpretation();

    //! \brief Resets the strings for the text forms and gloss forms from the database. If an id doesn't exist in the database, then a new text form or gloss is created.
    void loadStringsFromDatabase();

    //! \brief Resets the concordance's entries for this GlossItem (by text form and gloss)
    void updateGlossItemConcordance();

    //! \brief Set the candidate number status from what is stored in the database.
    void setCandidateNumberFromDatabase();

    void setTextFormNumberFromDatabase();

    void setGlossNumberFromDatabase();

    void setTextFormNumberFromDatabase(const WritingSystem & ws);

    void setGlossNumberFromDatabase(const WritingSystem & ws);

    QHash<WritingSystem,MorphologicalAnalysis*> mMorphologicalAnalyses;

    QHash<QString,Annotation> mAnnotations;

    CandidateNumber mCandidateNumber;
    ApprovalStatus mApprovalStatus;

    WritingSystem mBaselineWritingSystem;
    Project * mProject;
    DatabaseAdapter * mDbAdapter;
    Concordance * mConcordance;
    qlonglong mId;

    TextBitHash mTextForms;
    TextBitHash mGlosses;
    QHash<WritingSystem,bool> mMultipleTextForms;
    QHash<WritingSystem,bool> mMultipleGlosses;

    PartOfSpeech mPos;
};

#endif // GLOSSITEM_H
