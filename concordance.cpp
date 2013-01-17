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
    qlonglong oldId = mTextFormLingEdits.key( edit );
    mTextFormLingEdits.remove(oldId, edit);
    mTextFormLingEdits.insert(newTextFormId, edit);
}

void Concordance::updateGlossLingEditConcordance(LingEdit * edit, qlonglong newGlossId)
{
    qlonglong oldId = mGlossLingEdits.key( edit );
    mGlossLingEdits.remove(oldId, edit);
    mGlossLingEdits.insert(newGlossId, edit);
}

void Concordance::removeGlossFromLingEditConcordance( QObject * edit )
{
    LingEdit *lingEdit = qobject_cast<LingEdit*>(edit);
    qlonglong id = mGlossLingEdits.key( lingEdit );
    mGlossLingEdits.remove( id , lingEdit );
}

void Concordance::removeTextFormFromLingEditConcordance( QObject * edit )
{
    LingEdit *lingEdit = qobject_cast<LingEdit*>(edit);
    qlonglong id = mTextFormLingEdits.key( lingEdit );
    mTextFormLingEdits.remove( id , lingEdit );
}

void Concordance::updateGloss( const TextBit & bit )
{
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
    qlonglong oldId = mTextFormImmutableLabels.key( edit );
    mTextFormImmutableLabels.remove(oldId, edit);
    mTextFormImmutableLabels.insert(newTextFormId , edit);
}

void Concordance::updateGlossImmutableLabelConcordance(ImmutableLabel * edit, qlonglong newGlossId )
{
    qlonglong oldId = mGlossImmutableLabels.key( edit );
    mGlossImmutableLabels.remove(oldId, edit);
    mGlossImmutableLabels.insert(newGlossId , edit);
}

void Concordance::removeGlossFromImmutableLabelConcordance( QObject * edit )
{
    ImmutableLabel *label = qobject_cast<ImmutableLabel*>(edit);
    qlonglong id = mGlossImmutableLabels.key( label );
    mGlossImmutableLabels.remove( id , label );
}

void Concordance::removeTextFormFromImmutableLabelConcordance( QObject * edit )
{
    ImmutableLabel *label = qobject_cast<ImmutableLabel*>(edit);
    qlonglong id = mTextFormImmutableLabels.key( label );
    mTextFormImmutableLabels.remove( id , label );
}

void Concordance::removeGlossItemFromConcordance( QObject * item )
{
    GlossItem *glossItem = qobject_cast<GlossItem*>(item);
    qlonglong id = mGlossItemsByTextFormId.key( glossItem );
    mGlossItemsByTextFormId.remove( id, glossItem );
}

void Concordance::updateInterpretationsAvailableForGlossItem( GlossItem::CandidateNumber mCandidateNumber, qlonglong textFormId )
{
    QList<GlossItem*> itemList = mGlossItemsByTextFormId.values( textFormId );
    foreach(GlossItem *item, itemList)
        item->setCandidateNumber( mCandidateNumber );
}

void Concordance::updateGlossItemTextFormConcordance(GlossItem * item, qlonglong textFormId)
{
    qlonglong oldId = mGlossItemsByTextFormId.key( item );
    mGlossItemsByTextFormId.remove(oldId, item);
    mGlossItemsByTextFormId.insert(textFormId, item);
}

void Concordance::updateGlossItemMorphologicalAnalysis( const MorphologicalAnalysis & analysis, qlonglong textFormId )
{
    QList<GlossItem*> itemList = mGlossItemsByTextFormId.values( textFormId );
    foreach(GlossItem *item, itemList)
        item->setMorphologicalAnalysis( analysis.writingSystem(), analysis );
}
