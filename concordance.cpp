#include "concordance.h"

#include "lingedit.h"
#include "immutablelabel.h"

Concordance::Concordance(QObject *parent) :
    QObject(parent)
{
}

void Concordance::updateTextFormLingEditConcordance(LingEdit * edit, qlonglong newId)
{
    qlonglong oldId = mTextFormLingEdits.key( edit );
    mTextFormLingEdits.remove(oldId, edit);
    mTextFormLingEdits.insert(newId, edit);
}

void Concordance::updateGlossLingEditConcordance(LingEdit * edit, qlonglong newId)
{
    qlonglong oldId = mGlossLingEdits.key( edit );
    mGlossLingEdits.remove(oldId, edit);
    mGlossLingEdits.insert(newId, edit);
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

void Concordance::updateTextForImmutableLabelConcordance(ImmutableLabel * edit, qlonglong newId)
{
    qlonglong oldId = mTextFormImmutableLabels.key( edit );
    mTextFormImmutableLabels.remove(oldId, edit);
    mTextFormImmutableLabels.insert(newId, edit);
}

void Concordance::updateGlossImmutableLabelConcordance(ImmutableLabel * edit, qlonglong newId)
{
    qlonglong oldId = mGlossImmutableLabels.key( edit );
    mGlossImmutableLabels.remove(oldId, edit);
    mGlossImmutableLabels.insert(newId, edit);
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

void Concordance::updateGlossItemConcordance(GlossItem * item, qlonglong newId)
{
    qlonglong oldId = mGlossItems.key( item );
    mGlossItems.remove(oldId, item);
    mGlossItems.insert(newId, item);
}

void Concordance::removeGlossItemFromConcordance( GlossItem * item )
{
    qlonglong id = mGlossItems.key( item );
    mGlossItems.remove( id , item );
}

void Concordance::otherInterpretationsAvailableForGlossItem( qlonglong id )
{
    QList<GlossItem*> itemList = mGlossItems.values( id );
    foreach(GlossItem *item, itemList)
        item->setCandidateNumber( GlossItem::MultipleOption );
}
