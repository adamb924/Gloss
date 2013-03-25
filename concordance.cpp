#include "concordance.h"

#include "lingedit.h"
#include "immutablelabel.h"
#include "morphologicalanalysis.h"

#include <QtDebug>

Concordance::Concordance(QObject *parent) :
        QObject(parent)
{
}

void Concordance::removeGlossItemFromConcordance( QObject * item )
{
    QListIterator<qlonglong> keys( mGlossItemsByTextFormId.keys( (GlossItem*)item ) );
    while(keys.hasNext())
        mGlossItemsByTextFormId.remove( keys.next(), (GlossItem*)item );

    keys = QListIterator<qlonglong>( mGlossItemsByGlossId.keys( (GlossItem*)item ) );
    while(keys.hasNext())
        mGlossItemsByGlossId.remove( keys.next(), (GlossItem*)item );
}

void Concordance::removeGlossItemTextFormIdPair( GlossItem * item , qlonglong textFormId )
{
    mGlossItemsByTextFormId.remove(textFormId, item);
}

void Concordance::removeGlossItemGlossIdPair(GlossItem *item , qlonglong glossId )
{
    mGlossItemsByTextFormId.remove(glossId, item);
}

void Concordance::updateGloss( const TextBit & bit )
{
    // this is the new code to update the GlossItems specifically
    QList<GlossItem*> glossItems = mGlossItemsByGlossId.values( bit.id() );
    foreach(GlossItem *glossItem, glossItems)
        glossItem->setGlossText(bit);
}

void Concordance::updateTextForm( const TextBit & bit )
{
   QList<GlossItem*> glossItems = mGlossItemsByTextFormId.values( bit.id() );
    foreach(GlossItem *glossItem, glossItems)
        glossItem->setTextFormText(bit);
}

void Concordance::updateInterpretationsAvailableForGlossItem( GlossItem::CandidateNumber mCandidateNumber, qlonglong textFormId )
{
    QList<GlossItem*> itemList = mGlossItemsByTextFormId.values( textFormId );
    foreach(GlossItem *item, itemList)
        item->setCandidateNumber( mCandidateNumber );
}

void Concordance::updateGlossItemTextFormConcordance(GlossItem * item, qlonglong textFormId)
{
    mGlossItemsByTextFormId.remove( textFormId, item );
    mGlossItemsByTextFormId.insert(textFormId, item);
}

void Concordance::updateGlossItemGlossConcordance( GlossItem * item, qlonglong glossId )
{
    mGlossItemsByGlossId.remove( glossId, item );
    mGlossItemsByGlossId.insert( glossId, item );
}

void Concordance::updateGlossItemMorphologicalAnalysis( const MorphologicalAnalysis & analysis)
{
    // TODO another concordance problem
    QList<GlossItem*> itemList = mGlossItemsByTextFormId.values( analysis.textFormId() );
    foreach(GlossItem *item, itemList)
        item->setMorphologicalAnalysis( analysis );
}
