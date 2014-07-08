#ifndef MORPHEMEGRAPHICSITEM_H
#define MORPHEMEGRAPHICSITEM_H

#include <QGraphicsSimpleTextItem>

class TextBit;
class Allomorph;
class SyntacticAnalysisElement;

class MorphemeGraphicsItem : public QGraphicsSimpleTextItem
{
public:
    MorphemeGraphicsItem(const Allomorph * allomorph, SyntacticAnalysisElement * element, QGraphicsItem *parent = 0);

    const Allomorph *allomorph() const;
    SyntacticAnalysisElement* element();

private:
    const Allomorph *mAllomorph;
    SyntacticAnalysisElement * mElement;
};

#endif // MORPHEMEGRAPHICSITEM_H
