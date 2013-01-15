#include "immutablelabel.h"

#include "textbit.h"

ImmutableLabel::ImmutableLabel(const TextBit & bit, bool technicolor, QWidget *parent) :
        QLabel(parent)
{
    setText(bit.text());
    mTextBit = bit;
    mTechnicolor = technicolor;
    setToolTip(mTextBit.writingSystem().name());
    updateStyle();
}

void ImmutableLabel::setCandidateNumber(GlossItem::CandidateNumber status)
{
    mCandidateNumber = status;
    updateStyle();
}

void ImmutableLabel::setApprovalStatus(GlossItem::ApprovalStatus status)
{
    mApprovalStatus = status;
    updateStyle();
}

void ImmutableLabel::setTextBit(const TextBit & bit)
{
    mTextBit = bit;
    setToolTip(mTextBit.writingSystem().name());
    setText( bit.text() );
}

void ImmutableLabel::updateStyle()
{
    QString color;
    if( mTechnicolor )
    {
        if ( mApprovalStatus == GlossItem::Unapproved && mCandidateNumber == GlossItem::SingleOption )
            color = "#CDFFB2";
        else if ( mApprovalStatus == GlossItem::Unapproved && mCandidateNumber == GlossItem::MultipleOption )
            color = "#fff6a8";
        else
            color = "#ffffff";
    }
    else
    {
        color = "#ffffff";
    }
    setStyleSheet(QString("font-family: %1; font-size: %2pt; background-color: %3;").arg(mTextBit.writingSystem().fontFamily()).arg(mTextBit.writingSystem().fontSize()).arg(color));
}
