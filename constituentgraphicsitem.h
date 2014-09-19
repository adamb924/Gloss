#ifndef CONSTITUENTGRAPHICSITEM_H
#define CONSTITUENTGRAPHICSITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QList>
#include <QFont>
#include <QFontMetrics>

class MorphemeGraphicsItem;
class SyntacticAnalysisElement;

class ConstituentGraphicsItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    ConstituentGraphicsItem(const QString & label, const QList<QGraphicsItem*> daughters, SyntacticAnalysisElement * element, QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    SyntacticAnalysisElement* element();
    const SyntacticAnalysisElement* element() const;

    enum { Type = UserType + 1 };
    int type() const;

signals:
    void reparentElement( QList<SyntacticAnalysisElement *> elements, SyntacticAnalysisElement * newParent );

public slots:

private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent * event);

    int maxDepth() const;

    QList<QGraphicsItem*> mDaughters;
    QString mLabel;
    SyntacticAnalysisElement * mElement;

    QFont mFont;
    qreal mFontHeight;

    qreal mStalkHeight;
    qreal mPenWidth;

    int mDepth;
};

#endif // CONSTITUENTGRAPHICSITEM_H
