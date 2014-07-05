#ifndef MORPHEMEGRAPHICSITEM_H
#define MORPHEMEGRAPHICSITEM_H

#include <QGraphicsSimpleTextItem>

class TextBit;
class Allomorph;

class MorphemeGraphicsItem : public QGraphicsSimpleTextItem
{
public:
    MorphemeGraphicsItem(const Allomorph * allomorph, QGraphicsItem *parent = 0);

    const Allomorph *allomorph() const;

private:
    const Allomorph *mAllomorph;
};

#endif // MORPHEMEGRAPHICSITEM_H
