/*!
  \class ImmutableLabel
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
    ImmutableLabel(const TextBit & bit, bool technicolor, QWidget *parent = nullptr);
    ~ImmutableLabel();

    void matchTextAlignmentTo( Qt::LayoutDirection target );

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

signals:
    void doubleClick( TextBit & bit );

public slots:
    void setCandidateNumber(GlossItem::CandidateNumber status);
    void setApprovalStatus(GlossItem::ApprovalStatus status);
    void setCandidateNumberAndApprovalStatus(GlossItem::CandidateNumber number, GlossItem::ApprovalStatus status);
    void setTextBit(const TextBit & bit);

private:
    GlossItem::CandidateNumber mCandidateNumber;
    GlossItem::ApprovalStatus mApprovalStatus;
    TextBit mTextBit;
    bool mTechnicolor;

    void updateStyle();

    void mouseDoubleClickEvent ( QMouseEvent * event );
};

#endif // IMMUTABLELABEL_H
