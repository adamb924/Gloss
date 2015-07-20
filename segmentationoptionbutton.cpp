#include "segmentationoptionbutton.h"

#include <QMouseEvent>

SegmentationOptionButton::SegmentationOptionButton(const TextBit &text, bool allAttested, QWidget *parent) :
    QCommandLinkButton( text.text(), parent ),
    mTextBit( text )
{
    if( allAttested )
        setStyleSheet( QString("QCommandLinkButton { font-family: %1; font-weight: bold; }").arg(text.writingSystem().fontFamily()) );
    else
        setStyleSheet( QString("QCommandLinkButton { font-family: %1; font-weight: normal; }").arg(text.writingSystem().fontFamily()) );

    connect( this, SIGNAL(clicked(bool)), this, SLOT(emitClicked()) );
}

void SegmentationOptionButton::mouseReleaseEvent(QMouseEvent *event)
{
    QCommandLinkButton::mouseReleaseEvent(event);
    if( event->button() == Qt::RightButton )
    {
        emit rightClicked( mTextBit );
    }
}

void SegmentationOptionButton::emitClicked()
{
    emit clicked( mTextBit );
}

