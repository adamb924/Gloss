#include "concordance.h"

#include "lingedit.h"
#include "immutablelabel.h"
#include "morphologicalanalysis.h"

#include <QtDebug>

Concordance::Concordance(QObject *parent) :
        QObject(parent)
{
}

void Concordance::updateTextFormLingEditConcordance(LingEdit * edit, qlonglong newTextFormId)
{
    qDebug() << "Concordance::updateTextFormLingEditConcordance starting" << mTextFormLingEdits;
    removeTextFormFromLingEditConcordance( edit );
    mTextFormLingEdits.insert(newTextFormId, edit);
    qDebug() << "Concordance::updateTextFormLingEditConcordance ending" << mTextFormLingEdits;
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

void Concordance::removeGlossFromLingEditConcordance( QObject * edit )
{
    LingEdit *lingEdit = qobject_cast<LingEdit*>(edit);
    QListIterator<qlonglong> keys( mGlossLingEdits.keys( lingEdit ) );
    while(keys.hasNext())
        mGlossLingEdits.remove( keys.next() , lingEdit );
}

void Concordance::removeTextFormFromLingEditConcordance( QObject * edit )
{
    LingEdit *lingEdit = qobject_cast<LingEdit*>(edit);
    QListIterator<qlonglong> keys( mTextFormLingEdits.keys( lingEdit ) );
    while(keys.hasNext())
        mGlossLingEdits.remove( keys.next() , lingEdit );
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
    qlonglong oldId = mTextFormImmutableLabels.key( edit );
    mTextFormImmutableLabels.remove(oldId, edit);
    mTextFormImmutableLabels.insert(newTextFormId , edit);
}

void Concordance::updateGlossImmutableLabelConcordance(ImmutableLabel * edit, qlonglong newGlossId )
{
    // TODO fix this at some point
    qlonglong oldId = mGlossImmutableLabels.key( edit );
    mGlossImmutableLabels.remove(oldId, edit);
    mGlossImmutableLabels.insert(newGlossId , edit);
}

void Concordance::removeGlossFromImmutableLabelConcordance( QObject * edit )
{
    ImmutableLabel *label = qobject_cast<ImmutableLabel*>(edit);
    QListIterator<qlonglong> keys( mGlossImmutableLabels.keys( label ) );
    while(keys.hasNext())
        mGlossImmutableLabels.remove( keys.next() , label );
}

void Concordance::removeTextFormFromImmutableLabelConcordance( QObject * edit )
{
    ImmutableLabel *label = qobject_cast<ImmutableLabel*>(edit);
    QListIterator<qlonglong> keys( mTextFormImmutableLabels.keys( label ) );
    while(keys.hasNext())
        mTextFormImmutableLabels.remove( keys.next() , label );
}

void Concordance::removeGlossItemFromConcordance( QObject * item )
{
    GlossItem *glossItem = qobject_cast<GlossItem*>(item);
    QListIterator<qlonglong> keys( mGlossItemsByTextFormId.keys( glossItem ) );
    while(keys.hasNext())
        mGlossItemsByTextFormId.remove( keys.next(), glossItem );
}

void Concordance::updateInterpretationsAvailableForGlossItem( GlossItem::CandidateNumber mCandidateNumber, qlonglong textFormId )
{
    QList<GlossItem*> itemList = mGlossItemsByTextFormId.values( textFormId );
    foreach(GlossItem *item, itemList)
        item->setCandidateNumber( mCandidateNumber );
}

void Concordance::updateGlossItemTextFormConcordance(GlossItem * item, qlonglong textFormId)
{
    removeGlossItemFromConcordance( item );
    mGlossItemsByTextFormId.insert(textFormId, item);
}

void Concordance::updateGlossItemMorphologicalAnalysis( const MorphologicalAnalysis & analysis)
{
    QList<GlossItem*> itemList = mGlossItemsByTextFormId.values( analysis.textFormId() );
    foreach(GlossItem *item, itemList)
        item->setMorphologicalAnalysis( analysis );
}
