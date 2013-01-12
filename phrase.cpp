#include "phrase.h"
#include "glossitem.h"

#include <QtDebug>

Phrase::Phrase()
{
    mRequestGuiRefresh = true;
}

Phrase::~Phrase()
{
    // delete the gloss items
    qDeleteAll(*this);
}

TextBitHashIterator Phrase::glosses()
{
    return TextBitHashIterator(mGlosses);
}

TextBit Phrase::gloss(const WritingSystem & ws)
{
    if( !mGlosses.contains(ws))
        mGlosses.insert( ws, TextBit("", ws) );
    return mGlosses.value(ws);
}

void Phrase::setPhrasalGloss( const TextBit & bit )
{
    mGlosses.insert( bit.writingSystem(), bit );
}

QString Phrase::equivalentBaselineLineText() const
{
    QString text;
    for(int i=0; i<count(); i++)
    {
        text.append( at(i)->baselineText().text() );
        text.append(" ");
    }
    return text.trimmed();
}

bool Phrase::guiRefreshRequest() const
{
    return mRequestGuiRefresh;
}

void Phrase::setGuiRefreshRequest(bool needed)
{
    mRequestGuiRefresh = needed;
}

void Phrase::setAnnotation( const Annotation & annotation )
{
    mAnnotation = annotation;
}

Annotation* Phrase::annotation()
{
    return & mAnnotation;
}
