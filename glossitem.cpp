#include "glossitem.h"
#include "textbit.h"

#include <QtDebug>

GlossItem::GlossItem(const TextBit & baselineText, QObject *parent) : QObject(parent)
{
    mTextItems.insert(baselineText.writingSystem(), baselineText.text() );
    mBaselineText = baselineText;
    mId = -1;
}

GlossItem::GlossItem(const TextBit & baselineText, const QList<TextBit> & textForms, const QList<TextBit> & glossForms, QObject *parent) : QObject(parent)
{
    mBaselineText = baselineText;

    for(int i=0; i<textForms.count(); i++)
        mTextItems.insert( textForms.at(i).writingSystem() , textForms.at(i).text() );

    for(int i=0; i<glossForms.count(); i++)
        mGlossItems.insert( glossForms.at(i).writingSystem() , glossForms.at(i).text() );

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

TextBit GlossItem::baselineText() const
{
    return mBaselineText;
}

QHash<WritingSystem, QString>* GlossItem::textItems()
{
    return &mTextItems;
}

QHash<WritingSystem, QString>* GlossItem::glossItems()
{
    return &mGlossItems;
}
