#include "constituentgraphicsitem.h"

#include "morphemegraphicsitem.h"

#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QGraphicsItem>

#include <QtDebug>

ConstituentGraphicsItem::ConstituentGraphicsItem(const QString & label, const QList<QGraphicsItem*> daughters, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    mDaughters(daughters),
    mLabel(label)
{
    mStalkHeight = 10;
    mPenWidth = 2;
    mFont = QFont("Times New Roman",12);
    mFontHeight = QFontMetrics(mFont).height();
}

QRectF ConstituentGraphicsItem::boundingRect() const
{
//    qDebug() << "ConstituentGraphicsItem::boundingRect";

    if( mDaughters.isEmpty() )
    {
        return QRectF(0,0,0,0);
    }

    qreal left = mDaughters.first()->sceneBoundingRect().center().x();
    qreal right = mDaughters.first()->sceneBoundingRect().center().x();
    for(int i=1; i<mDaughters.count(); i++)
    {
        left = qMin( left, mDaughters.at(i)->sceneBoundingRect().center().x() );
        right = qMax( right, mDaughters.at(i)->sceneBoundingRect().center().x() );
    }
    qreal bottom = mDaughters.first()->sceneBoundingRect().top();
    qreal top = bottom - mStalkHeight - mPenWidth - mFontHeight;

    return QRectF( sceneTransform().inverted().map( QPointF(left,top) ) , QSize(right-left, bottom-top) );
}

void ConstituentGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen( QColor(0,0,0) );

    if( mDaughters.isEmpty() )
    {
        return;
    }
    QRectF rect = sceneBoundingRect();
    qreal stalkBase = rect.bottom();
    qreal stalkTop = stalkBase - mStalkHeight;
    for(int i=0; i<mDaughters.count(); i++)
    {
        qreal x = mDaughters.at(i)->sceneBoundingRect().center().x();
        painter->drawLine( sceneTransform().inverted().map( QPointF(x, stalkBase) ) ,sceneTransform().inverted().map( QPointF(x, stalkTop) ) );
    }

    painter->drawLine( sceneTransform().inverted().map( QPointF(rect.left(), stalkTop) ) ,sceneTransform().inverted().map( QPointF(rect.right(), stalkTop) ) );
    painter->drawText( sceneTransform().inverted().mapRect( QRectF( QPointF(rect.left(), stalkTop-mFontHeight), QPointF(rect.right(), stalkTop) ) ) , Qt::AlignCenter , mLabel );
}
