/*!
  \class GlossItem
  \ingroup Data
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

class GlossItem : public QObject
{
    Q_OBJECT
public:
    enum CandidateStatus { SingleOption, MultipleOption };
    enum ApprovalStatus { Approved, Unapproved };

    //! \brief Construct a GlossItem that is empty except for the baseline TextBit.
    explicit GlossItem(const TextBit & baselineText, Project *project, QObject *parent = 0);

    //! \brief Construct a GlossItem with the given WritingSystem, and gloss and text forms.
    GlossItem(const WritingSystem & ws, const TextBitHash & textForms, const TextBitHash & glossForms, Project *project, QObject *parent = 0);

    //! \brief Construct a GlossItem with the id (and data) from the given id (corresponding to the _id row of the Interpretations SQL table).
    GlossItem(const WritingSystem & ws, qlonglong id, Project *project, QObject *parent = 0);

    //! \brief Sets the id of this GlossItem (corresponding to the _id row of the Interpretations SQL table), and sets the data for the GlossItem accordingly.
    void setInterpretation(qlonglong id);

    //! \brief Returns the id of the GlossItem (corresponding to the _id row of the Interpretations SQL table)
    qlonglong id() const;

    TextBit baselineText() const;

    TextBitHash* textItems();
    TextBitHash* glossItems();

    void setCandidateStatus(CandidateStatus status);
    void setApprovalStatus(ApprovalStatus status);

    ApprovalStatus approvalStatus() const;

signals:
    void candidateStatusChanged(CandidateStatus status);
    void approvalStatusChanged(ApprovalStatus status);

public slots:
    void updateGloss( const TextBit & bit );
    void updateText( const TextBit & bit );

private:
    TextBitHash mTextItems;
    TextBitHash mGlossItems;

    //! \brief Attempt to set the (interpretation) id of \a bit by querying the database for interpretations compatible with the text form TextBit.
    void guessInterpretation();

    //! \brief Attempt to set the (interpretation) id of \a bit by querying the database for interpretations compatible with text and gloss TextBits.
    void guessInterpretation(const QList<TextBit> & textForms , const QList<TextBit> & glossForms);

    Project *mProject;

    CandidateStatus mCandidateStatus;
    ApprovalStatus mApprovalStatus;

    qlonglong mId;
    WritingSystem mBaselineWritingSystem;
};

#endif // GLOSSITEM_H
