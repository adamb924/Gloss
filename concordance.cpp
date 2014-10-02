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
    QListIterator<qlonglong> keys( mGlossItemsByInterpretationId.keys( (GlossItem*)item ) );
    while(keys.hasNext())
        mGlossItemsByInterpretationId.remove( keys.next(), (GlossItem*)item );

    keys = QListIterator<qlonglong>( mGlossItemsByTextFormId.keys( (GlossItem*)item ) );
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

    mGlossItemsByInterpretationId.insert( item->id(), item );
}

void Concordance::updateGlossItemMorphologicalAnalysis(const GlossItem * originator, const MorphologicalAnalysis * analysis)
{
    QList<GlossItem*> itemList = mGlossItemsByTextFormId.values( analysis->textFormId() );
    foreach(GlossItem *item, itemList)
    {
        /// this isn't necessary for the other concordance functions, but this
        /// one in particular needs to avoid unecessary copies of the MorphologicalAnalysis
        /// or else it will always be creating new GUIDs for the allomorphs.
        if( item != originator)
        {
            item->setMorphologicalAnalysis( analysis->copyWithNewGuids() );
        }
    }
}

void Concordance::insertIntoAllomorphConcordance(Allomorph *allomorph)
{
    mAllomorphsByLexicalEntryId.insert( allomorph->lexicalEntryId(), allomorph );
}

void Concordance::updateGlossItemConcordance(GlossItem *item)
{
    mGlossItemsByInterpretationId.remove( mGlossItemsByInterpretationId.key(item), item );
    mGlossItemsByInterpretationId.insert( item->id() , item );
}

void Concordance::removeFromAllomorphConcordance(Allomorph *allomorph)
{
    mAllomorphsByLexicalEntryId.remove( allomorph->lexicalEntryId(), allomorph );
}

void Concordance::updateAllomorphTextForms(Allomorph *allomorph)
{
    QList<Allomorph*> allomorphList = mAllomorphsByLexicalEntryId.values( allomorph->lexicalEntryId() );
    foreach( Allomorph * a, allomorphList )
    {
        if( a != allomorph ) // save a few cycles
        {
            a->setGlosses( allomorph->glosses() );
        }
    }
}

void Concordance::updateTextFormNumber(bool multipleAvailable, qlonglong interpretationId, const WritingSystem &ws)
{
    QList<GlossItem*> glossItems = mGlossItemsByInterpretationId.values( interpretationId );
    qDebug() << interpretationId << glossItems << ws << multipleAvailable;
    foreach(GlossItem *glossItem, glossItems)
        glossItem->setMultipleTextFormsAvailable( ws, multipleAvailable );
}

void Concordance::updateGlossNumber(bool multipleAvailable, qlonglong interpretationId, const WritingSystem &ws)
{
    QList<GlossItem*> glossItems = mGlossItemsByInterpretationId.values( interpretationId );
    foreach(GlossItem *glossItem, glossItems)
        glossItem->setMultipleGlossesAvailable( ws, multipleAvailable );
}
