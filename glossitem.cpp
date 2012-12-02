#include "glossitem.h"

GlossItem::GlossItem(TextBit *baselineText, QObject *parent) :
    QObject(parent)
{
    mBaselineText = baselineText;
    mId = -1;
}

void GlossItem::setId(qlonglong id)
{
    mId = id;
}

qlonglong GlossItem::id() const
{
    return mId;
}

TextBit* GlossItem::baselineText() const
{
    return mBaselineText;
}
