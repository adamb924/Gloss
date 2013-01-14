#include "phrase.h"
#include "glossitem.h"

#include <QtDebug>

Phrase::Phrase(DatabaseAdapter *dbAdapter)
{
    mDbAdapter = dbAdapter;
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

void Phrase::splitGlossInTwo( GlossItem *glossItem, const TextBit & wordOne, const TextBit & wordTwo )
{
    GlossItem *one = new GlossItem( wordOne, mDbAdapter );
    GlossItem *two = new GlossItem( wordTwo, mDbAdapter );

    int index = indexOf( glossItem );
    if( index != -1 )
    {
        delete at(index);
        replace(index, two);
        insert( index , one );
        mRequestGuiRefresh = true;
        emit phraseChanged();
    }
}
