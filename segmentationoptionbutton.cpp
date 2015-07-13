#include "segmentationoptionbutton.h"

#include <QMouseEvent>

SegmentationOptionButton::SegmentationOptionButton(const TextBit &text, QWidget *parent) :
    QCommandLinkButton( text.text(), parent ),
    mTextBit( text )
{
    setFont( mTextBit.writingSystem().font() );
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

