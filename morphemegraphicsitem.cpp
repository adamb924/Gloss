#include "morphemegraphicsitem.h"

#include "textbit.h"
#include "allomorph.h"
#include "syntacticanalysiselementmime.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QMimeData>
#include <QDrag>
#include <QGraphicsSceneMouseEvent>

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

int MorphemeGraphicsItem::type() const
{
    return Type;
}

void MorphemeGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    SyntacticAnalysisElementMime *data = new SyntacticAnalysisElementMime(mElement);
    QDrag *drag = new QDrag(event->widget());
    drag->setMimeData(data);
    drag->start();
}
