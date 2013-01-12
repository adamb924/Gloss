#include "phrasedisplaywidget.h"

#include "flowlayout.h"

#include "flowlayout.h"
#include "interlinearlinelabel.h"
#include "worddisplaywidget.h"
#include "databaseadapter.h"
#include "text.h"
#include "interlineardisplaywidget.h"

PhraseDisplayWidget::PhraseDisplayWidget(int index, Phrase *phrase, Text * text, InterlinearDisplayWidget *idw, DatabaseAdapter *dbAdapter,  QWidget *parent) :
        QWidget(parent)
{
    mIndex = index;
    mPhrase = phrase;
    mDbAdapter = dbAdapter;
    mText = text;
    mIdw = idw;

    mInterlinearDisplayLines = mDbAdapter->glossInterlinearLines();

    mFlowLayout = new FlowLayout( mInterlinearDisplayLines.first().writingSystem().layoutDirection() , 0, 5 , 5 , 5 );
    setLayout(mFlowLayout);

    addLineLabel();
    addWordWidgets();

    mPhrase->setGuiRefreshRequest(false);
}

void PhraseDisplayWidget::addLineLabel()
{
    InterlinearLineLabel *lineNumber = new InterlinearLineLabel(mIndex, QString("%1").arg(mIndex+1), this);
    connect(lineNumber, SIGNAL(approveAll(int)), this, SLOT(approveAll()));
    connect(lineNumber, SIGNAL(playSound(int)), this, SLOT(playSound()));
    mFlowLayout->addWidget(lineNumber);
}

void PhraseDisplayWidget::addWordWidgets()
{
    for(int i=0; i<mPhrase->count(); i++)
    {
        WordDisplayWidget *wdw = addWordDisplayWidget(mPhrase->at(i));
        connect( mPhrase->at(i), SIGNAL(interpretationIdChanged(qlonglong)), wdw, SLOT(sendConcordanceUpdates()) );
        connect( wdw, SIGNAL(alternateInterpretationAvailableFor(int)), this, SLOT(otherInterpretationsAvailableFor(int)) );
        mFlowLayout->addWidget(wdw);
    }
}

WordDisplayWidget* PhraseDisplayWidget::addWordDisplayWidget(GlossItem *item)
{
    WordDisplayWidget *wdw = new WordDisplayWidget( item , mText->baselineWritingSystem().layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight, mInterlinearDisplayLines , mIdw, mDbAdapter );
    mWordDisplayWidgets << wdw;
    mWdwByInterpretationId.insert( item->id() , wdw );
    mTextFormConcordance.unite( wdw->textFormEdits() );
    mGlossConcordance.unite( wdw->glossEdits() );

    if( mIdw != 0 )
    {
        connect( wdw, SIGNAL(glossIdChanged(LingEdit*,qlonglong)), mIdw, SLOT(updateGlossFormConcordance(LingEdit*,qlonglong)));
        connect( wdw, SIGNAL(textFormIdChanged(LingEdit*,qlonglong)), mIdw, SLOT(updateTextFormConcordance(LingEdit*,qlonglong)));
    }

    return wdw;
}

void PhraseDisplayWidget::approveAll()
{
    for(int i=0; i < mPhrase->count(); i++)
        mPhrase->at(i)->setApprovalStatus(GlossItem::Approved);
}

void PhraseDisplayWidget::playSound()
{
    // TODO implement this
}

void PhraseDisplayWidget::otherInterpretationsAvailableFor(int id)
{
    QList<WordDisplayWidget*> list = mWdwByInterpretationId.values(id);
    QListIterator<WordDisplayWidget*> iterator(list);
    while(iterator.hasNext())
        iterator.next()->glossItem()->setCandidateNumber(GlossItem::MultipleOption);
}
