#include "immutablelabel.h"

#include "textbit.h"

#include <QtDebug>

ImmutableLabel::ImmutableLabel(const TextBit & bit, bool technicolor, QWidget *parent) :
        QLabel(parent),
        mTextBit(bit),
        mTechnicolor(technicolor)
{
    setAlignment( Qt::AlignLeft );

    if( bit.writingSystem().layoutDirection() == Qt::RightToLeft )
    {
        setText( bit.text().prepend( QChar(0x202E) ) );
    }
    else
    {
        setText(bit.text());
    }
    setFont( mTextBit.writingSystem().font() );
    setToolTip(mTextBit.writingSystem().name());
    updateStyle();
}

ImmutableLabel::~ImmutableLabel()
{
}

QSize ImmutableLabel::sizeHint() const
{
    QSize sizeHint = QLabel::sizeHint();
    sizeHint.setWidth( fontMetrics().boundingRect(mTextBit.text()).width() + fontMetrics().averageCharWidth() );
    return sizeHint;
}

QSize ImmutableLabel::minimumSizeHint() const
{
    QSize sizeHint = QLabel::sizeHint();
    sizeHint.setWidth( fontMetrics().boundingRect(mTextBit.text()).width() + fontMetrics().averageCharWidth() );
    return sizeHint;
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

void ImmutableLabel::setCandidateNumberAndApprovalStatus(GlossItem::CandidateNumber number, GlossItem::ApprovalStatus status)
{
    mCandidateNumber = number;
    mApprovalStatus = status;
    updateStyle();
}

void ImmutableLabel::setTextBit(const TextBit & bit)
{
    mTextBit = bit;
    setFont( mTextBit.writingSystem().font() );
    setToolTip(mTextBit.writingSystem().name());
    setText( bit.text() );
    updateStyle();
}

void ImmutableLabel::updateStyle()
{
    if( mTextBit.writingSystem().isNull() ) return;

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

    setStyleSheet(QString("QLabel { font-family: %1; font-size: %2pt; background-color: %3; border-radius: 8px; }").arg(mTextBit.writingSystem().fontFamily()).arg(mTextBit.writingSystem().fontSize()).arg(color));
}

void ImmutableLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    emit doubleClick(mTextBit);
}
