#include "concordance.h"

#include "lingedit.h"
#include "immutablelabel.h"
#include "morphologicalanalysis.h"

#include <QtDebug>

Concordance::Concordance(QObject *parent) :
        QObject(parent)
{
}

void Concordance::removeGlossFromLingEditConcordance( QObject * edit )
{
    QListIterator<qlonglong> keys( mGlossLingEdits.keys( (LingEdit*)edit ) );
    while(keys.hasNext())
        mGlossLingEdits.remove( keys.next() , (LingEdit*)edit );
}

void Concordance::removeTextFormFromLingEditConcordance( QObject * edit )
{
    QListIterator<qlonglong> keys( mTextFormLingEdits.keys( (LingEdit*)edit ) );
    while(keys.hasNext())
        mTextFormLingEdits.remove( keys.next() , (LingEdit*)edit );
}

void Concordance::removeTextFormFromImmutableLabelConcordance( QObject * edit )
{
    QListIterator<qlonglong> keys( mTextFormImmutableLabels.keys( (ImmutableLabel*)edit ) );
    while(keys.hasNext())
        mTextFormImmutableLabels.remove( keys.next() , (ImmutableLabel*)edit );
}

void Concordance::removeGlossItemFromConcordance( QObject * item )
{
    QListIterator<qlonglong> keys( mGlossItems.keys( (GlossItem*)item ) );
    while(keys.hasNext())
        mGlossItems.remove( keys.next(), (GlossItem*)item );
}

void Concordance::removeGlossItemTextFormIdPair( GlossItem * item , qlonglong textFormId )
{
    mGlossItems.remove(textFormId, item);
}

void Concordance::removeGlossFromImmutableLabelConcordance( QObject * edit )
{
    QListIterator<qlonglong> keys( mGlossImmutableLabels.keys( (ImmutableLabel*)edit ) );
    while(keys.hasNext())
        mGlossImmutableLabels.remove( keys.next() , (ImmutableLabel*)edit );
}

void Concordance::updateTextFormLingEditConcordance(LingEdit * edit, qlonglong newTextFormId)
{
    removeTextFormFromLingEditConcordance( edit );
    mTextFormLingEdits.insert(newTextFormId, edit);
}

void Concordance::updateGlossLingEditConcordance(LingEdit * edit, qlonglong newGlossId)
{
    removeGlossFromLingEditConcordance(edit);
    mGlossLingEdits.insert(newGlossId, edit);
}


void Concordance::updateTextFormLingEditConcordance(const TextBit & bit, LingEdit * edit)
{
    updateTextFormLingEditConcordance( edit, bit.id() );
}

void Concordance::updateGlossLingEditConcordance( const TextBit & bit, LingEdit * edit)
{
    updateGlossLingEditConcordance( edit, bit.id() );
}

void Concordance::updateGloss( const TextBit & bit )
{
    // somehow the edit here is invalid
    QList<LingEdit*> editList = mGlossLingEdits.values(bit.id());
    foreach(LingEdit *edit, editList)
        edit->setTextBit(bit);

    QList<ImmutableLabel*> labelList = mGlossImmutableLabels.values(bit.id());
    foreach(ImmutableLabel *label, labelList)
        label->setTextBit( bit );
}

void Concordance::updateTextForm( const TextBit & bit )
{
    QList<LingEdit*> editList = mTextFormLingEdits.values(bit.id());
    foreach(LingEdit *edit, editList)
        edit->setTextBit( bit );

    QList<ImmutableLabel*> labelList = mTextFormImmutableLabels.values(bit.id());
    foreach(ImmutableLabel *label, labelList)
        label->setTextBit( bit );
}

void Concordance::updateTextForImmutableLabelConcordance(ImmutableLabel * edit, qlonglong newTextFormId )
{
    // TODO fix this at some point
    // TODO make a note of what is wrong with this!
    qlonglong oldId = mTextFormImmutableLabels.key( edit );
    mTextFormImmutableLabels.remove(oldId, edit);
    mTextFormImmutableLabels.insert(newTextFormId , edit);
}

void Concordance::updateGlossImmutableLabelConcordance(ImmutableLabel * edit, qlonglong newGlossId )
{
    // TODO fix this at some point
    // TODO make a note of what is wrong with this!
    qlonglong oldId = mGlossImmutableLabels.key( edit );
    mGlossImmutableLabels.remove(oldId, edit);
    mGlossImmutableLabels.insert(newGlossId , edit);
}

void Concordance::updateInterpretationsAvailableForGlossItem( GlossItem::CandidateNumber mCandidateNumber, qlonglong textFormId )
{
    QList<GlossItem*> itemList = mGlossItems.values( textFormId );
    foreach(GlossItem *item, itemList)
        item->setCandidateNumber( mCandidateNumber );
}

void Concordance::updateGlossItemTextFormConcordance(GlossItem * item, qlonglong textFormId)
{
    mGlossItems.remove( textFormId, item );
    mGlossItems.insert(textFormId, item);
}

void Concordance::updateGlossItemMorphologicalAnalysis( const MorphologicalAnalysis & analysis)
{
    // TODO another concordance problem
    QList<GlossItem*> itemList = mGlossItems.values( analysis.textFormId() );
    foreach(GlossItem *item, itemList)
        item->setMorphologicalAnalysis( analysis );
}
