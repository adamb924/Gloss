/*!
  \class ImmutableLabel
  \ingroup GUI
  \ingroup Interlinear
  \brief This QLabel subclass renders the specified TextBit as suggested by the font specifications in the associated WritingSystem. It is used mainly by WordDisplayWidget, but also by AnalysisWidget.

  The class also has various options to control its display, which are used by WordDisplayWidget.
*/

#ifndef IMMUTABLELABEL_H
#define IMMUTABLELABEL_H

#include <QLabel>

#include "textbit.h"
#include "glossitem.h"

class ImmutableLabel : public QLabel
{
    Q_OBJECT
public:
    ImmutableLabel(const TextBit & bit, bool technicolor, QWidget *parent = 0);
    ~ImmutableLabel();

signals:

public slots:
    void setCandidateNumber(GlossItem::CandidateNumber status);
    void setApprovalStatus(GlossItem::ApprovalStatus status);
    void setTextBit(const TextBit & bit);

private:
    GlossItem::CandidateNumber mCandidateNumber;
    GlossItem::ApprovalStatus mApprovalStatus;
    TextBit mTextBit;
    bool mTechnicolor;

    void updateStyle();
};

#endif // IMMUTABLELABEL_H
