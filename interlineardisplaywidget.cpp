#include "interlineardisplaywidget.h"

#include <QtDebug>
#include <QtGui>
#include <QStringList>
#include <QLayout>

#include "flowlayout.h"
#include "textbit.h"
#include "worddisplaywidget.h"
#include "text.h"
#include "project.h"
#include "databaseadapter.h"
#include "phrase.h"

InterlinearDisplayWidget::InterlinearDisplayWidget(Text *text, Project *project, QWidget *parent) :
    QWidget(parent)
{
    mText = text;
    mProject = project;
    mLayout = new QVBoxLayout(this);
    setLayout(mLayout);

    mPhrasalGlossLines = mProject->dbAdapter()->phrasalGlossLines();

    connect( text, SIGNAL(baselineTextChanged(QString)), this, SLOT(baselineTextUpdated(QString)));

    if( mText->phrases()->length() > 0 )
        setLayoutFromText();
}

InterlinearDisplayWidget::~InterlinearDisplayWidget()
{
    clearData();
}

void InterlinearDisplayWidget::baselineTextUpdated(const QString & baselineText)
{
    // TODO it's not clear to me that this is the best way to do this
    setLayoutFromText();
}

void InterlinearDisplayWidget::clearData()
{
    qDeleteAll(mWordDisplayWidgets);
    mWordDisplayWidgets.clear();

    qDeleteAll(mLineLayouts);
    mLineLayouts.clear();

    qDeleteAll(mPhrasalGlossEdits);
    mPhrasalGlossEdits.clear();

    mGlossConcordance.clear();
    mTextFormConcordance.clear();
}

void InterlinearDisplayWidget::setLayoutFromText()
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
        addPhrasalGlossLines(i);

        mText->phrases()->at(i)->setGuiRefreshRequest(false);
    }
    progress.setValue(mText->phrases()->count());
    mLayout->addStretch(100);
}

void InterlinearDisplayWidget::clearWidgets(QLayout * layout)
{
    if( layout->count() == 0 )
        return;
    QLayoutItem * item;
    while( ( item = layout->takeAt(0) ) != 0 )
    {
        mWordDisplayWidgets.remove(item->widget());
        delete item->widget();
        delete item;
    }
}

void InterlinearDisplayWidget::addLineLabel( int i , QLayout * flowLayout  )
{
    QLabel *lineNumber = new QLabel(QString("%1").arg(i+1), this);
    lineNumber->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lineNumber->setMinimumSize(30, 30);
    flowLayout->addWidget(lineNumber);
}

void InterlinearDisplayWidget::addWordDisplayWidgets( int i , QLayout * flowLayout )
{
    for(int j=0; j<mText->phrases()->at(i)->count(); j++)
    {
        WordDisplayWidget *wdw = addWordDisplayWidget(mText->phrases()->at(i)->at(j));
        connect( mText->phrases()->at(i)->at(j), SIGNAL(interpretationIdChanged(qlonglong)), wdw, SLOT(sendConcordanceUpdates()) );
        flowLayout->addWidget(wdw);
    }
}

void InterlinearDisplayWidget::addPhrasalGlossLines( int i )
{
    for(int j=0; j<mPhrasalGlossLines.count(); j++)
    {
        TextBit bit = mText->phrases()->at(i)->gloss( mPhrasalGlossLines.at(j).writingSystem() );
        LingEdit *edit = addPhrasalGlossLine( bit );
        edit->matchTextAlignmentTo( mText->baselineWritingSystem().layoutDirection() );
        connect( edit, SIGNAL(stringChanged(TextBit)), mText->phrases()->at(i), SLOT(setPhrasalGloss(TextBit)) );
    }
}

LingEdit* InterlinearDisplayWidget::addPhrasalGlossLine( const TextBit & gloss )
{
    LingEdit *edit = new LingEdit( gloss , this);
    mLayout->addWidget(edit);
    mPhrasalGlossEdits << edit;
    return edit;
}

WordDisplayWidget* InterlinearDisplayWidget::addWordDisplayWidget(GlossItem *item)
{
    WordDisplayWidget *wdw = new WordDisplayWidget( item , mText->baselineWritingSystem().layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight , this, mProject->dbAdapter() );
    mWordDisplayWidgets << wdw;
    mTextFormConcordance.unite( wdw->textFormEdits() );
    mGlossConcordance.unite( wdw->glossEdits() );

    connect( wdw, SIGNAL(glossIdChanged(LingEdit*,qlonglong)), this, SLOT(updateGlossFormConcordance(LingEdit*,qlonglong)));
    connect( wdw, SIGNAL(textFormIdChanged(LingEdit*,qlonglong)), this, SLOT(updateTextFormConcordance(LingEdit*,qlonglong)));

    return wdw;
}

QLayout* InterlinearDisplayWidget::addLine()
{
    FlowLayout *flowLayout = new FlowLayout( mText->baselineWritingSystem().layoutDirection() , 0, 5 , 5 , 5 );
    mLineLayouts << flowLayout;
    mLayout->addLayout(flowLayout);
    return flowLayout;
}

void InterlinearDisplayWidget::updateGloss( const TextBit & bit )
{
    LingEdit* edit;
    QList<LingEdit*> editList = mGlossConcordance.values(bit.id());
    foreach(edit, editList)
        edit->setTextBit(bit);
}

void InterlinearDisplayWidget::updateText( const TextBit & bit )
{
    LingEdit* edit;
    QList<LingEdit*> editList = mTextFormConcordance.values(bit.id());
    foreach(edit, editList)
        edit->setTextBit( bit );
}

void InterlinearDisplayWidget::updateMorphologicalAnalysis( const TextBit & bit , const QString & splitString )
{

}

void InterlinearDisplayWidget::updateTextFormConcordance(LingEdit * edit, qlonglong newId)
{
    qlonglong oldId = mTextFormConcordance.key( edit );
    mTextFormConcordance.remove(oldId, edit);
    mTextFormConcordance.insert(newId, edit);
}

void InterlinearDisplayWidget::updateGlossFormConcordance(LingEdit * edit, qlonglong newId)
{
    qlonglong oldId = mGlossConcordance.key( edit );
    mGlossConcordance.remove(oldId, edit);
    mGlossConcordance.insert(newId, edit);
}
