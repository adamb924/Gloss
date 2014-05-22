#include "phrase.h"
#include "glossitem.h"
#include "project.h"
#include "text.h"
#include "generictextinputdialog.h"

#include <QMessageBox>
#include <QtDebug>

Phrase::Phrase(Text *text, Project *project)
{
    mText = text;
    mProject = project;
    mConcordance = mProject->concordance();
    mDbAdapter = mProject->dbAdapter();
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
    if( !mGlosses.contains(bit.writingSystem()) || mGlosses.value(bit.writingSystem()) !=  bit )
    {
        mGlosses.insert( bit.writingSystem(), bit );
        emit glossChanged();
    }
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

void Phrase::setInterval( const Interval & annotation )
{
    mAnnotation = annotation;
}

Interval* Phrase::interval()
{
    return & mAnnotation;
}

void Phrase::splitGloss( GlossItem *glossItem, const QList<TextBit> & bits )
{
    int index = mGlossItems.indexOf( glossItem );
    if( index == -1 )
        return;

    glossItem->deleteLater();

    mGlossItems.removeAt(index);
    for(int i=0; i<bits.count(); i++)
    {
        GlossItem *tmp = connectGlossItem( new GlossItem( bits.at(i) , mProject ));
        mGlossItems.insert(index, tmp);
        index++;
    }
    emit requestGuiRefresh(this);
    emit phraseChanged();
}

void Phrase::mergeGlossItemWithNext( GlossItem *glossItem )
{
    int index = mGlossItems.indexOf( glossItem );
    if( index == -1 || index >= glossItemCount() -1 )
        return;

    GenericTextInputDialog dialog( TextBit( glossItemAt(index)->baselineText().text() + glossItemAt(index+1)->baselineText().text() , glossItem->baselineWritingSystem() ) , 0);
    dialog.setWindowTitle(tr("Edit the baseline text, if necessary. Any spaces will be converted to non-breaking spaces."));
    if( dialog.exec() == QDialog::Accepted )
    {
        TextBit newBit = dialog.textBit();

        QString replacement = newBit.text();
        replacement.replace(QChar(0x0020), QChar(0x00A0));
        newBit.setText(replacement);

        GlossItem *newGlossItem = connectGlossItem( new GlossItem( newBit, mProject ) );
        mGlossItems.replace( index, newGlossItem );

        GlossItem* toRemove = mGlossItems.takeAt( index+1 );
        toRemove->deleteLater();
        glossItem->deleteLater();

        emit requestGuiRefresh(this);
        emit phraseChanged();
    }
}

void Phrase::mergeGlossItemWithPrevious( GlossItem *glossItem )
{
    int index = mGlossItems.indexOf( glossItem );
    if( index <= 0 || index >= glossItemCount() )
        return;

    GenericTextInputDialog dialog( TextBit( glossItemAt(index-1)->baselineText().text() + glossItemAt(index)->baselineText().text() , glossItem->baselineWritingSystem() ) , 0);
    dialog.setWindowTitle(tr("Edit the baseline text, if necessary. Any spaces will be converted to non-breaking spaces."));
    if( dialog.exec() == QDialog::Accepted )
    {
        TextBit newBit = dialog.textBit();

        QString replacement = newBit.text();
        replacement.replace(QChar(0x0020), QChar(0x00A0));
        newBit.setText(replacement);

        GlossItem *newGlossItem = connectGlossItem( new GlossItem( newBit, mProject ) );
        mGlossItems.replace( index, newGlossItem );

        GlossItem* toRemove = mGlossItems.takeAt( index-1 );
        toRemove->deleteLater();
        glossItem->deleteLater();

        emit requestGuiRefresh(this);
        emit phraseChanged();
    }
}

void Phrase::removeGlossItem( GlossItem *glossItem )
{
    if( mGlossItems.count() == 1)
    {
        if( QMessageBox::Yes == QMessageBox::question(0, tr("Remove phrase?"), tr("This is the last gloss item on the line. Would you like to remove the entire phrase? (If you click no, nothing will happen.)"), QMessageBox::Yes | QMessageBox::No , QMessageBox::No ) )
        {
            emit requestRemovePhrase(this);
        }
        return;
    }

    GlossItem* toRemove = (GlossItem*)glossItem;
    int index = mGlossItems.indexOf( toRemove );
    if( index >= 0 && index < glossItemCount() )
    {
        mGlossItems.removeAt(index);
        toRemove->deleteLater();

        emit requestGuiRefresh(this);
        emit phraseChanged();
    }
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

GlossItem* Phrase::connectGlossItem(GlossItem * item)
{
    item->connectToConcordance();
    connect( item, SIGNAL(baselineTextChanged(TextBit)), this, SIGNAL(phraseChanged()) );
    connect( item, SIGNAL(fieldsChanged()), mText, SLOT(markAsChanged()) );
    connect( item, SIGNAL(approvalStatusChanged(GlossItem::ApprovalStatus)), mText, SLOT(markAsChanged()) );
    return item;
}

void Phrase::appendGlossItem(GlossItem * item)
{
    mGlossItems << connectGlossItem(item);
}

GlossItem* Phrase::lastGlossItem()
{
    return mGlossItems.last();
}

const QList<GlossItem*>* Phrase::glossItems() const
{
    return &mGlossItems;
}

int Phrase::indexOfGlossItem(GlossItem * item) const
{
    return mGlossItems.indexOf(item);
}
