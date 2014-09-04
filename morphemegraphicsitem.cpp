#include "morphemegraphicsitem.h"

#include "textbit.h"
#include "allomorph.h"

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
    QMimeData *data = new QMimeData;
    data->setData("SyntacticAnalysisElement*", QByteArray((char*)mElement) );

    QDrag *drag = new QDrag(event->widget());
    drag->setMimeData(data);
    drag->start();
}
