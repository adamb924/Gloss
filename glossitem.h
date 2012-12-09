/*!
  \class GlossItem
  \ingroup DataStructures
  \brief A data class representing a gloss item in an interlinear text. This consists of an id (the _id field of the Interpretations SQL table), hashes of text and gloss lines, and a pointer to the TextBit of the baseline text.
*/

#ifndef GLOSSITEM_H
#define GLOSSITEM_H

#include <QObject>
#include <QHash>

#include "writingsystem.h"
#include "textbit.h"

class TextBit;
class Project;
class DatabaseAdapter;

class GlossItem : public QObject
{
    Q_OBJECT
public:
    enum CandidateStatus { SingleOption, MultipleOption };
    enum ApprovalStatus { Approved, Unapproved };

    //! \brief Construct a GlossItem that is empty except for the baseline TextBit.
    explicit GlossItem(const TextBit & baselineText, DatabaseAdapter *dbAdapter, QObject *parent = 0);

    //! \brief Construct a GlossItem with the given WritingSystem, and gloss and text forms.
    GlossItem(const WritingSystem & ws, const TextBitHash & textForms, const TextBitHash & glossForms, DatabaseAdapter *dbAdapter, QObject *parent = 0);

    //! \brief Construct a GlossItem with the id (and data) from the given id (corresponding to the _id row of the Interpretations SQL table).
    GlossItem(const WritingSystem & ws, qlonglong id, DatabaseAdapter *dbAdapter, QObject *parent = 0);

    //! \brief Sets the id of this GlossItem (corresponding to the _id row of the Interpretations SQL table), and sets the data for the GlossItem accordingly.
    void setInterpretation(qlonglong id);

    void setGloss(const TextBit & gloss);
    void setTextForm(const TextBit & textForm);

    //! \brief Returns the id of the GlossItem (corresponding to the _id row of the Interpretations SQL table)
    qlonglong id() const;

    TextBit baselineText() const;

    TextBitHash* textForms();
    TextBitHash* glosses();

    //! \brief Returns the text form for the given writing system. If it does not exist, a new one is added to the database, an the corresponding TextBit is returned.
    TextBit textForm(const WritingSystem & ws);

    //! \brief Returns the gloss for the given writing system. If it does not exist, a new one is added to the database, an the corresponding TextBit is returned.
    TextBit gloss(const WritingSystem & ws);

    void setCandidateStatus(CandidateStatus status);
    void setApprovalStatus(ApprovalStatus status);

    ApprovalStatus approvalStatus() const;
    CandidateStatus candidateStatus() const;

    WritingSystem writingSystem() const;

signals:
    void candidateStatusChanged(CandidateStatus status);
    void approvalStatusChanged(ApprovalStatus status);
    void interpretationIdChanged(qlonglong id);
    void fieldsChanged();

public slots:
    void updateGloss( const TextBit & bit );
    void updateText( const TextBit & bit );
    void toggleApproval();

private:
    TextBitHash mTextForms;
    TextBitHash mGlosses;

    //! \brief Attempt to set the (interpretation) id of \a bit by querying the database for interpretations compatible with the text forms and gloss forms, or if there are none, than for those compatible with the baseline bit. If no compatible interpretation is found, a new interpretation is created.
    void guessInterpretation();

    DatabaseAdapter *mDbAdapter;

    CandidateStatus mCandidateStatus;
    ApprovalStatus mApprovalStatus;

    qlonglong mId;
    WritingSystem mBaselineWritingSystem;
};

#endif // GLOSSITEM_H
