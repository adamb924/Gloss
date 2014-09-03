#include "morphemegraphicsitem.h"

#include "textbit.h"
#include "allomorph.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

MorphemeGraphicsItem::MorphemeGraphicsItem(const TextBit & bit, SyntacticAnalysisElement *element, QGraphicsItem * parent)
    : QGraphicsSimpleTextItem( bit.text(), parent),
    mElement(element)
{
    setFont( bit.writingSystem().font() );
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

SyntacticAnalysisElement *MorphemeGraphicsItem::element()
{
    return mElement;
}
