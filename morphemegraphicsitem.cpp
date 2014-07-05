#include "morphemegraphicsitem.h"

#include "textbit.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

MorphemeGraphicsItem::MorphemeGraphicsItem(const TextBit & bit, QGraphicsItem * parent)
    : QGraphicsSimpleTextItem(bit.text(), parent)
{
    setFont( bit.writingSystem().font() );
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}
