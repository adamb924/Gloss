#include "glossdisplaywidget.h"

#include <QtGui>

#include "project.h"
#include "text.h"
#include "lingedit.h"
#include "worddisplaywidget.h"
#include "databaseadapter.h"

GlossDisplayWidget::GlossDisplayWidget(Text *text, Project *project, QWidget *parent) : InterlinearDisplayWidget(text, project, parent)
{
    mPhrasalGlossLines = mProject->glossPhrasalGlossLines();

    connect( text, SIGNAL(baselineTextChanged(QString)), this, SLOT(baselineTextUpdated(QString)));

    if( mText->phrases()->length() > 0 )
        setLayoutFromText();
}

void GlossDisplayWidget::updateGloss( const TextBit & bit )
{
    LingEdit* edit;
    QList<LingEdit*> editList = mGlossConcordance.values(bit.id());
    foreach(edit, editList)
        edit->setTextBit(bit);
}

void GlossDisplayWidget::updateText( const TextBit & bit )
{
    LingEdit* edit;
    QList<LingEdit*> editList = mTextFormConcordance.values(bit.id());
    foreach(edit, editList)
        edit->setTextBit( bit );
}

void GlossDisplayWidget::updateTextFormConcordance(LingEdit * edit, qlonglong newId)
{
    qlonglong oldId = mTextFormConcordance.key( edit );
    mTextFormConcordance.remove(oldId, edit);
    mTextFormConcordance.insert(newId, edit);
}

void GlossDisplayWidget::updateGlossFormConcordance(LingEdit * edit, qlonglong newId)
{
    qlonglong oldId = mGlossConcordance.key( edit );
    mGlossConcordance.remove(oldId, edit);
    mGlossConcordance.insert(newId, edit);
}

void GlossDisplayWidget::removeGlossFromConcordance( LingEdit * edit )
{
    qlonglong id = mGlossConcordance.key( edit );
    mGlossConcordance.remove( id , edit );
}

void GlossDisplayWidget::removeTextFormFromConcordance( LingEdit * edit )
{
    qlonglong id = mTextFormConcordance.key( edit );
    mTextFormConcordance.remove( id , edit );
}

void GlossDisplayWidget::otherInterpretationsAvailableFor(int id)
{
    QList<WordDisplayWidget*> list = mWdwByInterpretationId.values(id);
    QListIterator<WordDisplayWidget*> iterator(list);
    while(iterator.hasNext())
        iterator.next()->glossItem()->setCandidateNumber(GlossItem::MultipleOption);
}

void GlossDisplayWidget::baselineTextUpdated(const QString & baselineText)
{
    setLayoutFromText();
}

void GlossDisplayWidget::setLayoutFromText()
{
    //    clearData();
    QProgressDialog progress(tr("Creating interface for %1...").arg(mText->name()), "Cancel", 0, mText->phrases()->count(), 0);
    progress.setWindowModality(Qt::WindowModal);

    for(int i=0; i< mText->phrases()->count(); i++)
    {
        progress.setValue(i);

        QLayout *flowLayout;

        if( i >= mLineLayouts.count() ) // there is no layout here
        {
            flowLayout = addLine();
            addPhrasalGlossLines(i);
        }
        else if( mText->phrases()->at(i)->guiRefreshRequest() )
        {
            flowLayout = mLineLayouts.at(i);
            clearWidgets( flowLayout );
        }
        else
        {
            continue;
        }

        addLineLabel(i, flowLayout);
        if( flowLayout->count() == 1 ) // it's either new or has been cleared for a refresh
            addWordDisplayWidgets(i, flowLayout);

        mText->phrases()->at(i)->setGuiRefreshRequest(false);
    }
    progress.setValue(mText->phrases()->count());
}

void GlossDisplayWidget::addWordDisplayWidgets( int i , QLayout * flowLayout )
{
    for(int j=0; j<mText->phrases()->at(i)->count(); j++)
    {
        WordDisplayWidget *wdw = addWordDisplayWidget(mText->phrases()->at(i)->at(j));
        connect( mText->phrases()->at(i)->at(j), SIGNAL(interpretationIdChanged(qlonglong)), wdw, SLOT(sendConcordanceUpdates()) );
        connect( wdw, SIGNAL(alternateInterpretationAvailableFor(int)), this, SLOT(otherInterpretationsAvailableFor(int)) );
        flowLayout->addWidget(wdw);
    }
}

void GlossDisplayWidget::clearData()
{
    InterlinearDisplayWidget::clearData();

    qDeleteAll(mWordDisplayWidgets);
    mWordDisplayWidgets.clear();

    // do not call qDeleteAll again because they've just been deleted. this is a convenience container.
    mWdwByInterpretationId.clear();

    qDeleteAll(mPhrasalGlossEdits);
    mPhrasalGlossEdits.clear();

    mGlossConcordance.clear();
    mTextFormConcordance.clear();
}

WordDisplayWidget* GlossDisplayWidget::addWordDisplayWidget(GlossItem *item)
{
    WordDisplayWidget *wdw = new WordDisplayWidget( item , mText->baselineWritingSystem().layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight, mProject->glossInterlinearLines() , this, mProject->dbAdapter() );
    mWordDisplayWidgets << wdw;
    mWdwByInterpretationId.insert( item->id() , wdw );
    mTextFormConcordance.unite( wdw->textFormEdits() );
    mGlossConcordance.unite( wdw->glossEdits() );

    connect( wdw, SIGNAL(glossIdChanged(LingEdit*,qlonglong)), this, SLOT(updateGlossFormConcordance(LingEdit*,qlonglong)));
    connect( wdw, SIGNAL(textFormIdChanged(LingEdit*,qlonglong)), this, SLOT(updateTextFormConcordance(LingEdit*,qlonglong)));

    return wdw;
}

void GlossDisplayWidget::addPhrasalGlossLines( int i )
{
    for(int j=0; j<mPhrasalGlossLines.count(); j++)
    {
        TextBit bit = mText->phrases()->at(i)->gloss( mPhrasalGlossLines.at(j).writingSystem() );
        LingEdit *edit = addPhrasalGlossLine( bit );
        edit->matchTextAlignmentTo( mText->baselineWritingSystem().layoutDirection() );
        connect( edit, SIGNAL(stringChanged(TextBit)), mText->phrases()->at(i), SLOT(setPhrasalGloss(TextBit)) );
    }
}

LingEdit* GlossDisplayWidget::addPhrasalGlossLine( const TextBit & gloss )
{
    LingEdit *edit = new LingEdit( gloss , this);
    mLayout->addWidget(edit);
    mPhrasalGlossEdits << edit;
    return edit;
}

void GlossDisplayWidget::clearWidgets(QLayout * layout)
{
    if( layout->count() == 0 )
        return;
    QLayoutItem * item;
    while( ( item = layout->takeAt(0) ) != 0 )
    {
        mWordDisplayWidgets.remove(item->widget());
        mWdwByInterpretationId.remove( mWdwByInterpretationId.key(qobject_cast<WordDisplayWidget*>(item->widget())) );
        delete item->widget();
        delete item;
    }
}
