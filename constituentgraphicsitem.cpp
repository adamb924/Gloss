#include "constituentgraphicsitem.h"

#include "morphemegraphicsitem.h"
#include "syntacticanalysiselementmime.h"

#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QMimeData>
#include <QDrag>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

#include "syntacticanalysiselement.h"
#include "allomorph.h"

#include <QtDebug>

ConstituentGraphicsItem::ConstituentGraphicsItem(const QString & label, const QList<QGraphicsItem*> daughters, SyntacticAnalysisElement * element, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    mDaughters(daughters),
    mLabel(label),
    mElement(element),
    mFont("Times New Roman",12),
    mStalkHeight(10),
    mPenWidth(2)
{
    mFontHeight = QFontMetrics(mFont).height();
    mDepth =  maxDepth();

    setAcceptDrops(true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QRectF ConstituentGraphicsItem::boundingRect() const
{
    if( mDaughters.isEmpty() )
    {
        return QRectF(0,0,0,0);
    }

    qreal left = mDaughters.first()->sceneBoundingRect().center().x();
    qreal right = mDaughters.first()->sceneBoundingRect().center().x();
    QPointF pt = sceneTransform().inverted().map( QPointF(0, 0 ) );
    qreal bottom = pt.y();

    for(int i=0; i<mDaughters.count(); i++)
    {
        left = qMin( left, mDaughters.at(i)->sceneBoundingRect().left() );
        right = qMax( right, mDaughters.at(i)->sceneBoundingRect().right() );
    }

    qreal top = bottom - mDepth * ( mPenWidth + mFontHeight + mStalkHeight );

    return QRectF( sceneTransform().inverted().map( QPointF(left,top) ) , QSizeF(right-left, bottom-top) );
}

QPainterPath ConstituentGraphicsItem::shape() const
{
    QPainterPath path;
    QRectF rect = sceneBoundingRect();
    qreal stalkTop = rect.top() + mFontHeight;

    QRectF textRect = QRectF( QPointF(rect.left(), stalkTop-mFontHeight), QPointF(rect.right(), stalkTop) );

    QRectF selectionRect = QFontMetrics(mFont).boundingRect( mLabel );
    selectionRect.moveCenter( textRect.center() );
    selectionRect.adjust( 0 , 0, 0 , -2 );

    path.addRect( selectionRect );
    return path;
}

void ConstituentGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    painter->setPen( QPen(QBrush(QColor(0,0,0)), mPenWidth) );

    if( mDaughters.isEmpty() )
    {
        return;
    }
    QRectF rect = sceneBoundingRect();

    /// draw the stalks
    qreal stalkTop = rect.top() + mFontHeight;
    qreal left = mDaughters.first()->sceneBoundingRect().center().x();
    qreal right = mDaughters.first()->sceneBoundingRect().center().x();

    for(int i=0; i<mDaughters.count(); i++)
    {
        qreal x = mDaughters.at(i)->sceneBoundingRect().center().x();
        painter->drawLine( sceneTransform().inverted().map( QPointF(x, mDaughters.at(i)->sceneBoundingRect().top() ) ) ,sceneTransform().inverted().map( QPointF(x, stalkTop) ) );

        left = qMin( left, mDaughters.at(i)->sceneBoundingRect().center().x() );
        right = qMax( right, mDaughters.at(i)->sceneBoundingRect().center().x() );
    }

    /// draw the horizontal line
    painter->drawLine( sceneTransform().inverted().map( QPointF(left, stalkTop) ) ,sceneTransform().inverted().map( QPointF(right, stalkTop) ) );

    /// draw the text label
    QRectF textRect;
    if( rect.left() == rect.right() )
    {
        textRect = QFontMetrics(mFont).boundingRect( mLabel );
        textRect.moveCenter( QPointF(rect.left() , stalkTop-mFontHeight/2) );
    }
    else
    {
        textRect = QRectF( QPointF(rect.left(), stalkTop-mFontHeight), QPointF(rect.right(), stalkTop) );
    }
    painter->drawText( sceneTransform().inverted().mapRect( textRect ) , Qt::AlignCenter , mLabel );

    /// draw selection rectangle
    if(option->state & QStyle::State_Selected)
    {
        QRectF selectionRect = QFontMetrics(mFont).boundingRect( mLabel );
        selectionRect.moveCenter( textRect.center() );
        selectionRect.adjust( 0 , 0, 0 , -2 ); /// pull it away from the horizontal line
        painter->setPen( Qt::DashLine );
        painter->drawRect( selectionRect );
    }
}

void ConstituentGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QList<QGraphicsItem *> items = scene()->selectedItems();
    QList<SyntacticAnalysisElement *> selection;

    if( items.isEmpty() )
    {
        selection.append(mElement);
    }
    else
    {
        foreach(QGraphicsItem * item, items)
        {
            MorphemeGraphicsItem * mgi = qgraphicsitem_cast<MorphemeGraphicsItem*>(item);
            if( mgi != nullptr )
            {
                selection.append( mgi->element() );
            }
            else
            {
                ConstituentGraphicsItem * cgi = qgraphicsitem_cast<ConstituentGraphicsItem*>(item);
                if ( cgi != nullptr )
                {
                    selection.append( cgi->element() );
                }
            }
        }
    }

    SyntacticAnalysisElementMime *data = new SyntacticAnalysisElementMime(selection);
    QDrag *drag = new QDrag(event->widget());
    drag->setMimeData(data);
    drag->exec();
}

void ConstituentGraphicsItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if( event->source() != nullptr ) /// if it's not coming from outside the application
    {
        const SyntacticAnalysisElementMime *data = qobject_cast<const SyntacticAnalysisElementMime*>(event->mimeData());
        if( data != nullptr )
        {
            emit reparentElement( data->elements() , mElement );
        }
    }
}

int ConstituentGraphicsItem::maxDepth() const
{
    int depth = 0;
    foreach( QGraphicsItem* item , mDaughters )
    {
        ConstituentGraphicsItem * con = qgraphicsitem_cast<ConstituentGraphicsItem*>(item);
        if( con != nullptr )
        {
            depth = qMax( depth , con->maxDepth() );
        }
    }
    return depth + 1;
}

SyntacticAnalysisElement *ConstituentGraphicsItem::element()
{
    return mElement;
}

const SyntacticAnalysisElement *ConstituentGraphicsItem::element() const
{
    return mElement;
}

int ConstituentGraphicsItem::type() const
{
    return Type;
}
