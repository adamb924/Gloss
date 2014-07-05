#include "morphemegraphicsitem.h"

#include "textbit.h"
#include "allomorph.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

MorphemeGraphicsItem::MorphemeGraphicsItem(const Allomorph *allomorph, QGraphicsItem * parent)
    : QGraphicsSimpleTextItem( allomorph->textBitForConcatenation().text(), parent),
    mAllomorph(allomorph)
{
    setFont( allomorph->writingSystem().font() );
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

const Allomorph* MorphemeGraphicsItem::allomorph() const
{
    return mAllomorph;
}
