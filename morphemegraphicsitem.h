#ifndef MORPHEMEGRAPHICSITEM_H
#define MORPHEMEGRAPHICSITEM_H

#include <QGraphicsSimpleTextItem>

class TextBit;

class MorphemeGraphicsItem : public QGraphicsSimpleTextItem
{
public:
    MorphemeGraphicsItem(const TextBit &bit, QGraphicsItem *parent = 0);

};

#endif // MORPHEMEGRAPHICSITEM_H
