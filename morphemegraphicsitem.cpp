#include "morphemegraphicsitem.h"

#include "textbit.h"
#include "allomorph.h"
#include "syntacticanalysiselementmime.h"
#include "constituentgraphicsitem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QMimeData>
#include <QDrag>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

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
    QList<QGraphicsItem *> items = scene()->selectedItems();
    QList<SyntacticAnalysisElement *> selection;

    qDebug() << "MorphemeGraphicsItem::mousePressEvent" << mElement;

    if( items.isEmpty() )
    {
        selection.append(mElement);
    }
    else
    {
        foreach(QGraphicsItem * item, items)
        {
            MorphemeGraphicsItem * mgi = qgraphicsitem_cast<MorphemeGraphicsItem*>(item);
            if( mgi != 0 )
            {
                selection.append( mgi->element() );
            }
            else
            {
                ConstituentGraphicsItem * cgi = qgraphicsitem_cast<ConstituentGraphicsItem*>(item);
                if ( cgi != 0 )
                {
                    selection.append( cgi->element() );
                }
            }
        }
    }

    SyntacticAnalysisElementMime *data = new SyntacticAnalysisElementMime(selection);
    QDrag *drag = new QDrag(event->widget());
    drag->setMimeData(data);
    drag->start();
}
