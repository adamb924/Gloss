#include "phrase.h"
#include "glossitem.h"
#include "project.h"

#include <QtDebug>

Phrase::Phrase(Project *project)
{
    mProject = project;
    mDbAdapter = mProject->dbAdapter();
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
    GlossItem *one = new GlossItem( wordOne, mProject );
    GlossItem *two = new GlossItem( wordTwo, mProject );

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

void Phrase::mergeGlossItemWithNext( GlossItem *glossItem )
{
    int index = indexOf( glossItem );
    if( index == -1 || index >= count() -1 )
        return;
    TextBit newBit = TextBit( at(index)->baselineText().text() + at(index+1)->baselineText().text()  ,  glossItem->baselineWritingSystem() );
    GlossItem *newGlossItem = new GlossItem( newBit, mProject );
    replace( index, newGlossItem );
    delete takeAt( index+1 );
    mRequestGuiRefresh = true;
    emit phraseChanged();
}

void Phrase::mergeGlossItemWithPrevious( GlossItem *glossItem )
{
    int index = indexOf( glossItem );
    if( index <= 0 || index >= count() )
        return;
    TextBit newBit = TextBit( at(index-1)->baselineText().text() + at(index)->baselineText().text()  ,  glossItem->baselineWritingSystem() );
    GlossItem *newGlossItem = new GlossItem( newBit, mProject );
    replace( index, newGlossItem );
    delete takeAt( index-1 );
    mRequestGuiRefresh = true;
    emit phraseChanged();
}
