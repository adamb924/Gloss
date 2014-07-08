#include "morphemegraphicsitem.h"

#include "textbit.h"
#include "allomorph.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

MorphemeGraphicsItem::MorphemeGraphicsItem(const Allomorph *allomorph, SyntacticAnalysisElement *element, QGraphicsItem * parent)
    : QGraphicsSimpleTextItem( allomorph->textBitForConcatenation().text(), parent),
    mAllomorph(allomorph),
    mElement(element)
{
    setFont( allomorph->writingSystem().font() );
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

const Allomorph* MorphemeGraphicsItem::allomorph() const
{
    return mAllomorph;
}

SyntacticAnalysisElement *MorphemeGraphicsItem::element()
{
    return mElement;
}
