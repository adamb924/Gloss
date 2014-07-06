#ifndef CONSTITUENTGRAPHICSITEM_H
#define CONSTITUENTGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QList>
#include <QFont>
#include <QFontMetrics>

class MorphemeGraphicsItem;

class ConstituentGraphicsItem : public QGraphicsItem
{
public:
    ConstituentGraphicsItem(const QString & label, const QList<QGraphicsItem*> daughters, QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:

public slots:

private:
    QList<QGraphicsItem*> mDaughters;
    QString mLabel;

    QFont mFont;
    qreal mFontHeight;

    qreal mStalkHeight;
    qreal mPenWidth;
};

#endif // CONSTITUENTGRAPHICSITEM_H
