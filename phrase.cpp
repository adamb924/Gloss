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
    qDeleteAll(mGlossItems);
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
    for(int i=0; i<glossItemCount(); i++)
    {
        text.append( glossItemAt(i)->baselineText().text() );
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

    int index = mGlossItems.indexOf( glossItem );
    if( index != -1 )
    {
        delete mGlossItems.at(index);
        mGlossItems.replace(index, two);
        mGlossItems.insert( index , one );
        mRequestGuiRefresh = true;
//        emit phraseChanged();
    }
}

void Phrase::mergeGlossItemWithNext( GlossItem *glossItem )
{
    int index = mGlossItems.indexOf( glossItem );
    if( index == -1 || index >= glossItemCount() -1 )
        return;
    TextBit newBit = TextBit( glossItemAt(index)->baselineText().text() + glossItemAt(index+1)->baselineText().text()  ,  glossItem->baselineWritingSystem() );
    GlossItem *newGlossItem = new GlossItem( newBit, mProject );
    mGlossItems.replace( index, newGlossItem );
    delete mGlossItems.takeAt( index+1 );
    mRequestGuiRefresh = true;
//    emit phraseChanged();
}

void Phrase::mergeGlossItemWithPrevious( GlossItem *glossItem )
{
    int index = mGlossItems.indexOf( glossItem );
    if( index <= 0 || index >= glossItemCount() )
        return;
    TextBit newBit = TextBit( glossItemAt(index-1)->baselineText().text() + glossItemAt(index)->baselineText().text()  ,  glossItem->baselineWritingSystem() );
    GlossItem *newGlossItem = new GlossItem( newBit, mProject );
    mGlossItems.replace( index, newGlossItem );
    delete mGlossItems.takeAt( index-1 );
    mRequestGuiRefresh = true;
//    emit phraseChanged();
}

int Phrase::glossItemCount() const
{
    return mGlossItems.count();
}

const GlossItem* Phrase::glossItemAt(int index) const
{
    return mGlossItems.at(index);
}

GlossItem* Phrase::glossItemAt(int index)
{
    return mGlossItems[index];
}

void Phrase::clearGlossItems()
{
    qDeleteAll(mGlossItems);
    mGlossItems.clear();
}

void Phrase::appendGlossItem(GlossItem * item)
{
    mGlossItems << item;
}

GlossItem* Phrase::lastGlossItem()
{
    return mGlossItems.last();
}
