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

signals:

public slots:
    void setCandidateNumber(GlossItem::CandidateNumber status);
    void setApprovalStatus(GlossItem::ApprovalStatus status);

private:
    GlossItem::CandidateNumber mCandidateNumber;
    GlossItem::ApprovalStatus mApprovalStatus;
    TextBit mTextBit;
    bool mTechnicolor;

    void updateStyle();
};

#endif // IMMUTABLELABEL_H
