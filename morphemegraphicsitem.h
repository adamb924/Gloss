#ifndef MORPHEMEGRAPHICSITEM_H
#define MORPHEMEGRAPHICSITEM_H

#include <QGraphicsSimpleTextItem>

class TextBit;
class Allomorph;
class SyntacticAnalysisElement;

class MorphemeGraphicsItem : public QGraphicsSimpleTextItem
{
public:
    MorphemeGraphicsItem(const TextBit &bit, SyntacticAnalysisElement * element, QGraphicsItem *parent = 0);

    SyntacticAnalysisElement* element();

private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    SyntacticAnalysisElement * mElement;
};

#endif // MORPHEMEGRAPHICSITEM_H
