#include "linenumbergraphicsitem.h"

#include <QPainter>
#include <QtDebug>

LineNumberGraphicsItem::LineNumberGraphicsItem(int lineNumber) : mLineNumber(lineNumber), mFont(QFont("Times New Roman",12))
{

}

QRectF LineNumberGraphicsItem::boundingRect() const
{
    QRectF rect = QFontMetrics(QFont()).boundingRect( tr("%1").arg(mLineNumber) );
    rect.moveTop( -1 * rect.top() );
//    rect.moveLeft( -0.75 * rect.width() );
//    rect.moveRight( 0.75 * rect.width() );
    return rect;
}

void LineNumberGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->drawText( boundingRect(), tr("%1").arg(mLineNumber) );
}

void LineNumberGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    emit requestPlayLine(mLineNumber-1); /// Text::playSoundForLine expects a 0-indexed number
}
