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
#include "interlinearlinelabel.h"
#include "generictextinputdialog.h"

InterlinearDisplayWidget::InterlinearDisplayWidget(const QList<InterlinearItemType> & interlinearDisplayLines, const QList<InterlinearItemType> & phrasalGlossLines, Text *text, Project *project, QWidget *parent) :
        QScrollArea(parent)
{
    mText = text;
    mProject = project;
    mInterlinearDisplayLines = interlinearDisplayLines;
    mPhrasalGlossLines = phrasalGlossLines;

    setLinesToDefault();

    mCurrentLine = -1;

    mLayout = new QVBoxLayout;
    QWidget *theWidget = new QWidget(this);
    theWidget->setLayout(mLayout);

    setWidgetResizable(true);
    setBackgroundRole(QPalette::Light);
    this->setWidget( theWidget );

    setLayoutFromText();
}

InterlinearDisplayWidget::InterlinearDisplayWidget(const QList<InterlinearItemType> & interlinearDisplayLines, const QList<InterlinearItemType> & phrasalGlossLines, Text *text, Project *project, QList<int> lines, QWidget *parent)
{
    mText = text;
    mProject = project;
    mInterlinearDisplayLines = interlinearDisplayLines;
    mPhrasalGlossLines = phrasalGlossLines;
    mLines = lines;

    mCurrentLine = -1;

    mLayout = new QVBoxLayout;
    QWidget *theWidget = new QWidget(this);
    theWidget->setLayout(mLayout);

    setWidgetResizable(true);
    setBackgroundRole(QPalette::Light);
    this->setWidget( theWidget );

    setLayoutFromText();
}

InterlinearDisplayWidget::~InterlinearDisplayWidget()
{

}

void InterlinearDisplayWidget::addLineLabel( int i , QLayout * flowLayout  )
{
    InterlinearLineLabel *lineNumber = new InterlinearLineLabel(i, QString("%1").arg(i+1), mText->phrases()->at(i)->annotation()->isValid(), mText->phrases()->at(i)->annotation()->summaryString(), this);
    connect(lineNumber, SIGNAL(approveAll(int)), this, SLOT(approveAll(int)));
    connect(lineNumber, SIGNAL(playSound(int)), this, SLOT(playSound(int)));
    connect(lineNumber, SIGNAL(editLine(int)), this, SLOT(editLine(int)));

    flowLayout->addWidget(lineNumber);
    mLineLabels.insert(i, lineNumber);
}

QLayout* InterlinearDisplayWidget::addLine(int lineNumber)
{
    FlowLayout *flowLayout = new FlowLayout( mInterlinearDisplayLines.first().writingSystem().layoutDirection() , 0, 5 , 5 , 5 );
    mLineLayouts.insert(lineNumber, flowLayout);
    mLayout->addLayout(flowLayout);
    return flowLayout;
}

void InterlinearDisplayWidget::saveText()
{
    mText->saveText(false);
}

void InterlinearDisplayWidget::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);
    menu.addAction(tr("Save this text"), this, SLOT(saveText()), QKeySequence("Ctrl+Shift+S"));
    menu.exec(event->globalPos());
}

void InterlinearDisplayWidget::scrollToLine(int line)
{
    if( mLineLayouts.value(line) != 0 )
    {
        bool after = line > mCurrentLine;
        mCurrentLine = line;
        if( mLineLayouts.value(mCurrentLine)->count() > 1 )
        {
            if( after )
            {
                ensureWidgetVisible( mLineLayouts.value(mCurrentLine)->itemAt( mLineLayouts.value(mCurrentLine)->count() - 1 )->widget() , 0 , 300 );
            }
            else
            {
                ensureWidgetVisible( mLineLayouts.value(mCurrentLine)->itemAt(0)->widget() , 0 , 300 );
            }
        }
    }
}

void InterlinearDisplayWidget::scrollContentsBy ( int dx, int dy )
{
    QScrollArea::scrollContentsBy(dx, dy);
    if( dy != 0 )
    {

    }
}

void InterlinearDisplayWidget::addPhrasalGlossLines( int i )
{
    for(int j=0; j<mPhrasalGlossLines.count(); j++)
    {
        TextBit bit = mText->phrases()->at(i)->gloss( mPhrasalGlossLines.at(j).writingSystem() );
        LingEdit *edit = addPhrasalGlossLine( bit );
        edit->matchTextAlignmentTo( mText->baselineWritingSystem().layoutDirection() );
        connect( edit, SIGNAL(stringChanged(TextBit,LingEdit*)), mText->phrases()->at(i), SLOT(setPhrasalGloss(TextBit)) );
    }
}

LingEdit* InterlinearDisplayWidget::addPhrasalGlossLine( const TextBit & gloss )
{
    LingEdit *edit = new LingEdit( gloss , this);
    mLayout->addWidget(edit);
    return edit;
}

void InterlinearDisplayWidget::approveAll(int lineNumber)
{
    if( lineNumber >= mText->phrases()->count() )
        return;
    for(int i=0; i < mText->phrases()->at(lineNumber)->glossItemCount(); i++)
        mText->phrases()->at(lineNumber)->glossItemAt(i)->setApprovalStatus(GlossItem::Approved);
}

void InterlinearDisplayWidget::playSound(int lineNumber)
{
    mText->playSoundForLine(lineNumber);
}

void InterlinearDisplayWidget::editLine(int lineNumber)
{
    // Launch a dialog requesting input
    GenericTextInputDialog dialog( TextBit( mText->baselineTextForLine(lineNumber) , mText->baselineWritingSystem() ) , this);
    dialog.setWindowTitle(tr("Edit baseline text - Line %1").arg(lineNumber+1));
    if( dialog.exec() == QDialog::Accepted )
    {
        mText->setBaselineTextForLine(lineNumber, dialog.text() );
        setLayoutFromText();
    }
}

void InterlinearDisplayWidget::clearWidgetsFromLine(int lineNumber)
{
    QLayout *layout = mLineLayouts.value(lineNumber);
    if( layout != 0 )
    {

        QWidget *lineLabel = mLineLabels.take(lineNumber);
        if( lineLabel != 0 )
        {
            lineLabel->deleteLater();
            layout->removeWidget(lineLabel);
        }

        QListIterator<QWidget*> iter =  QListIterator<QWidget*>( mWordDisplayWidgets.values(lineNumber) );
        while(iter.hasNext())
        {
            QWidget *wdw = iter.next();
            layout->removeWidget(wdw);
            wdw->deleteLater();
            mWordDisplayWidgets.remove( lineNumber, wdw );
        }
    }
}

void InterlinearDisplayWidget::setLayoutFromText()
{
    if( mLines.isEmpty() )
        setLinesToDefault();

    QProgressDialog progress(tr("Creating interface for %1...").arg(mText->name()), "Cancel", 0, mLines.count(), 0);
    progress.setWindowModality(Qt::WindowModal);

    QListIterator<int> iter(mLines);
    while(iter.hasNext())
    {
        progress.setValue( progress.value() + 1 );

        int lineIndex = iter.next();

        QLayout *flowLayout;

        if( mLineLayouts.value(lineIndex) == 0 ) // there is no layout here
        {
            flowLayout = addLine(lineIndex);
            addPhrasalGlossLines(lineIndex);
        }
        else if( mLineRefreshRequests.contains( lineIndex ) )
        {
            flowLayout = mLineLayouts.value(lineIndex);
            clearWidgetsFromLine(lineIndex);
        }
        else
        {
            continue;
        }

        if( flowLayout->isEmpty() )
        {
            addLineLabel(lineIndex, flowLayout);
            addWordWidgets(lineIndex, flowLayout);
        }

        mLineRefreshRequests.clear();
    }
    progress.setValue(mLines.count());
}

void InterlinearDisplayWidget::addWordWidgets( int i , QLayout * flowLayout )
{
    for(int j=0; j<mText->phrases()->at(i)->glossItemCount(); j++)
    {
        WordDisplayWidget *wdw = addWordDisplayWidget(mText->phrases()->at(i)->glossItemAt(j), mText->phrases()->at(i));
        mWordDisplayWidgets.insert(i, wdw);
        flowLayout->addWidget(wdw);
    }
}

WordDisplayWidget* InterlinearDisplayWidget::addWordDisplayWidget(GlossItem *item, Phrase *phrase)
{
    WordDisplayWidget *wdw = new WordDisplayWidget( item , mText->baselineWritingSystem().layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight, mInterlinearDisplayLines, mProject->dbAdapter(), this );

    connect( wdw, SIGNAL(splitWidgetInTwo(GlossItem*,TextBit,TextBit)), phrase, SLOT(splitGlossInTwo(GlossItem*,TextBit,TextBit)) );
    connect( wdw, SIGNAL(mergeGlossItemWithNext(GlossItem*)), phrase, SLOT(mergeGlossItemWithNext(GlossItem*)));
    connect( wdw, SIGNAL(mergeGlossItemWithPrevious(GlossItem*)), phrase, SLOT(mergeGlossItemWithPrevious(GlossItem*)));
    connect( wdw, SIGNAL(requestRemoveGlossItem(GlossItem*)), phrase, SLOT(removeGlossItem(GlossItem*)));

    return wdw;
}

void InterlinearDisplayWidget::setLinesToDefault()
{
    mLines.clear();
    for(int i=0; i<mText->phrases()->count(); i++)
        mLines << i;
}

void InterlinearDisplayWidget::requestLineRefresh( int line )
{
    mLineRefreshRequests.insert( line );
}
