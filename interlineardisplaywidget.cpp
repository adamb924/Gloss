#include "interlineardisplaywidget.h"

#include <QtDebug>
#include <QtWidgets>
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
#include "focus.h"
#include "annotationmarkwidget.h"
#include "tab.h"
#include "writingsystem.h"

InterlinearDisplayWidget::InterlinearDisplayWidget(const Tab * tab, Text *text, Project *project, QWidget *parent) :
    QScrollArea(parent), mTab(tab), mText(text), mProject(project), mMouseMode(InterlinearDisplayWidget::Normal)
{
    mBottomSpacing = new QSpacerItem(0,0,QSizePolicy::Minimum,QSizePolicy::Expanding);

    setLinesToDefault();

    mCurrentLine = -1;

    mLayout = new QVBoxLayout;
    QWidget *theWidget = new QWidget(this);
    theWidget->setLayout(mLayout);

    setWidgetResizable(true);
    setBackgroundRole(QPalette::Light);
    this->setWidget( theWidget );
}

InterlinearDisplayWidget::~InterlinearDisplayWidget()
{
}

void InterlinearDisplayWidget::setPhrasalGloss(int lineNumber, const TextBit &bit)
{
    QList<LingEdit*> edits = mPhrasalGlossEdits.values(lineNumber);
    for( int i=0; i<edits.count(); i++)
    {
        if( edits.at(i)->textBit().writingSystem() == bit.writingSystem() )
        {
            edits.at(i)->setTextBit(bit);
        }
    }
}

void InterlinearDisplayWidget::addLineLabel( int i , QLayout * flowLayout  )
{
    InterlinearLineLabel *lineNumber = new InterlinearLineLabel(i, QString("%1").arg(i+1), mText->phrases()->at(i)->interval()->isValid(), mText->phrases()->at(i)->interval()->summaryString(), this);
    connect(lineNumber, SIGNAL(approveAll(int)), this, SLOT(approveAll(int)));
    connect(lineNumber, SIGNAL(playSound(int)), this, SLOT(playSound(int)));
    connect(lineNumber, SIGNAL(editLine(int)), this, SLOT(editLine(int)));
    connect(lineNumber, SIGNAL(removeLine(int)), mText, SLOT(removeLine(int)) );

    flowLayout->addWidget(lineNumber);
    mLineLabels.insert(i, lineNumber);
}

QLayout* InterlinearDisplayWidget::addLine(int lineNumber)
{
    FlowLayout *flowLayout = new FlowLayout( mTab->interlinearLines().value( mText->baselineWritingSystem())->first().writingSystem().layoutDirection() , 0, 5 , 5 , 5 );
    mLineLayouts.insert(lineNumber, flowLayout);
    mLayout->addLayout(flowLayout);
    return flowLayout;
}

void InterlinearDisplayWidget::saveText()
{
    mText->saveText(false, false, true, true);
}

void InterlinearDisplayWidget::saveTextVerbose()
{
    mText->saveText(true, true, true, true);
}

void InterlinearDisplayWidget::resetGui()
{
    QList<int> newLines = mLines;
    for(int i=0; i<newLines.count(); i++)
    {
        if( newLines.at(i) >= mText->phrases()->count() )
        {
            newLines.removeAt(i);
        }
    }
    setLines(newLines);
}

void InterlinearDisplayWidget::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);
    menu.addAction(tr("Save this text"), this, SLOT(saveText()), QKeySequence("Ctrl+Shift+S"));
    menu.addAction(tr("Save this text, verbose output"), this, SLOT(saveTextVerbose()) );

    QActionGroup *wsGroup = new QActionGroup(this);
    QMenu *wsMenu = new QMenu(tr("Change baseline writing system..."));
    QList<WritingSystem> ws = mProject->dbAdapter()->writingSystems();
    for(int i=0; i < ws.count(); i++)
    {
        QAction *action = new QAction(ws.at(i).name(),this);
        action->setData(ws.at(i).id());
        wsMenu->addAction(action);
        wsGroup->addAction(action);
    }
    menu.addMenu(wsMenu);

    connect(wsGroup,SIGNAL(triggered(QAction*)),this, SLOT(enterChangeBaselineMode(QAction*)) );

    menu.exec(event->globalPos());
}

void InterlinearDisplayWidget::enterChangeBaselineMode(QAction * action)
{
    mMouseMode = InterlinearDisplayWidget::ChangeBaseline;
    mChangeWritingSystemTo = mProject->dbAdapter()->writingSystem( action->data().toInt() );
    setCursor(QCursor(Qt::CrossCursor));
}

void InterlinearDisplayWidget::keyPressEvent ( QKeyEvent * event )
{
    int key = event->key();
    if( key == Qt::Key_Escape )
    {
        mMouseMode = InterlinearDisplayWidget::Normal;
        setCursor(QCursor(Qt::ArrowCursor));
    }
}

void InterlinearDisplayWidget::wdwClicked( WordDisplayWidget * wdw )
{
    if( mMouseMode == InterlinearDisplayWidget::ChangeBaseline )
    {
        TextBit newBaseline = wdw->glossItem()->baselineText();
        newBaseline.setWritingSystem( mChangeWritingSystemTo );
        int lineNumber = lineNumberOfWdw(wdw);
        mText->phrases()->at(lineNumber)->replaceGlossItem(wdw->glossItem(), newBaseline);
    }
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
    for(int j=0; j< mTab->phrasalGlossLines()->count(); j++)
    {
        TextBit bit = mText->phrases()->at(i)->gloss( mTab->phrasalGlossLines()->at(j).writingSystem() );
        LingEdit *edit = addPhrasalGlossLine( bit );
        edit->matchTextAlignmentTo( mTab->interlinearLines().value( mText->baselineWritingSystem())->first().writingSystem().layoutDirection() );
        connect( edit, SIGNAL(stringChanged(TextBit,LingEdit*)), mText->phrases()->at(i), SLOT(setPhrasalGloss(TextBit)) );

        mPhrasalGlossEdits.insert( i , edit );
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

        QListIterator<WordDisplayWidget*> iter( mWordDisplayWidgets.at(lineNumber) );
        while(iter.hasNext())
        {
            WordDisplayWidget *wdw = iter.next();
            layout->removeWidget(wdw);
            wdw->deleteLater();
            mWordDisplayWidgets[lineNumber].removeOne( wdw );
        }

        QListIterator<LingEdit*> phrasalIter( mPhrasalGlossEdits.values(lineNumber) );
        while(phrasalIter.hasNext())
        {
            LingEdit *gloss = phrasalIter.next();
            layout->removeWidget(gloss);
            gloss->deleteLater();
            mPhrasalGlossEdits.remove( lineNumber, gloss );
        }
   }
}

void InterlinearDisplayWidget::setLayoutFromText()
{
    if( mLines.isEmpty() )
        setLinesToDefault();

    mLayout->removeItem(mBottomSpacing);

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

    mLayout->addSpacerItem( mBottomSpacing );
}

void InterlinearDisplayWidget::addWordWidgets( int i , QLayout * flowLayout )
{
    for(int j=0; j<mText->phrases()->at(i)->glossItemCount(); j++)
    {
        WordDisplayWidget *wdw = addWordDisplayWidget(mText->phrases()->at(i)->glossItemAt(j), mText->phrases()->at(i));
        if( !( i < mWordDisplayWidgets.count() ) )
            mWordDisplayWidgets.append( QList<WordDisplayWidget*>() );
        mWordDisplayWidgets[i].append(wdw);
        flowLayout->addWidget(wdw);
    }
}

WordDisplayWidget* InterlinearDisplayWidget::addWordDisplayWidget(GlossItem *item, Phrase *phrase)
{
    WordDisplayWidget *wdw = new WordDisplayWidget( item , mText->baselineWritingSystem().layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight, mTab, mProject, this );
    maybeFocus(wdw);

    connect( wdw, SIGNAL(splitWidget(GlossItem*,QList<TextBit>)), phrase, SLOT(splitGloss(GlossItem*,QList<TextBit>)) );

    connect( wdw, SIGNAL(mergeGlossItemWithNext(GlossItem*)), phrase, SLOT(mergeGlossItemWithNext(GlossItem*)));
    connect( wdw, SIGNAL(mergeGlossItemWithPrevious(GlossItem*)), phrase, SLOT(mergeGlossItemWithPrevious(GlossItem*)));
    connect( wdw, SIGNAL(requestRemoveGlossItem(GlossItem*)), phrase, SLOT(removeGlossItem(GlossItem*)));

    connect( wdw, SIGNAL(requestApproveLine(WordDisplayWidget*)), this, SLOT(approveAll(WordDisplayWidget*)) );
    connect( wdw, SIGNAL(requestPlaySound(WordDisplayWidget*)), this, SLOT(playSound(WordDisplayWidget*)) );
    connect( wdw, SIGNAL(requestLeftGlossItem(WordDisplayWidget*)), this, SLOT(leftGlossItem(WordDisplayWidget*)));
    connect( wdw, SIGNAL(requestRightGlossItem(WordDisplayWidget*)), this, SLOT(rightGlossItem(WordDisplayWidget*)));

    connect( wdw, SIGNAL(requestSetFollowingInterpretations(GlossItem*)), mText, SLOT(setFollowingInterpretations(GlossItem*)) );
    connect( wdw, SIGNAL(requestReplaceFollowing(GlossItem*,QString)), mText, SLOT(replaceFollowing(GlossItem*,QString)) );
    connect( wdw, SIGNAL(requestSetFollowingTextForms(GlossItem*,WritingSystem)), mText, SLOT(matchFollowingTextForms(GlossItem*,WritingSystem)) );
    connect( wdw, SIGNAL(requestSetFollowingGlosses(GlossItem*,WritingSystem)), mText, SLOT(matchFollowingGlosses(GlossItem*,WritingSystem)) );

    connect( wdw, SIGNAL(leftClicked(WordDisplayWidget*)), this, SLOT(wdwClicked(WordDisplayWidget*)) );

    return wdw;
}

void InterlinearDisplayWidget::maybeFocus(WordDisplayWidget * wdw)
{
    bool isFocused = false;
    for(int i=0; i<mFoci.count(); i++)
    {
        if( wdw->glossItem()->matchesFocus( mFoci.at(i) ) )
        {
            isFocused = true;
            break;
        }
    }
    wdw->setFocused(isFocused);
}

void InterlinearDisplayWidget::setLinesToDefault()
{
    mLines.clear();
    for(int i=0; i<mText->phrases()->count(); i++)
        mLines << i;
}

void InterlinearDisplayWidget::setLines( const QList<int> lines )
{
    for(int i=0; i<mLines.count(); i++)
        clearWidgetsFromLine(mLines.at(i));
    mLines = lines;
    qDeleteAll(mLineLayouts);
    mLineLayouts.clear();
    mLineLabels.clear();
    setLayoutFromText();
}

void InterlinearDisplayWidget::requestLineRefresh( int line )
{
    mLineRefreshRequests.insert( line );
    setLayoutFromText();
}

void InterlinearDisplayWidget::setFocus( const QList<Focus> & foci )
{
    mFoci = foci;

    for( int i=0; i<mWordDisplayWidgets.count(); i++ )
    {
        for( int j=0; j<mWordDisplayWidgets.at(i).count(); j++)
        {
            maybeFocus( mWordDisplayWidgets.at(i).at(j) );
        }
    }
}

void InterlinearDisplayWidget::approveAll( WordDisplayWidget * wdw )
{
    int lineNumber = lineNumberOfWdw(wdw);
    if( lineNumber != -1 )
    {
        approveAll(lineNumber);
    }
}

void InterlinearDisplayWidget::playSound( WordDisplayWidget * wdw )
{
    int lineNumber = lineNumberOfWdw(wdw);
    if( lineNumber != -1 )
    {
        playSound(lineNumber);
    }
}

int InterlinearDisplayWidget::lineNumberOfWdw( WordDisplayWidget * wdw ) const
{
    for(int i=0; i<mWordDisplayWidgets.count(); i++)
    {
        if( mWordDisplayWidgets.at(i).contains(wdw) )
        {
            return i;
        }
    }
    return -1;
}

void InterlinearDisplayWidget::leftGlossItem( WordDisplayWidget * wdw )
{
    if( wdw->glossItem()->baselineWritingSystem().layoutDirection() == Qt::LeftToRight )
    {
        // next
        moveToNextGlossItem(wdw);
    }
    else
    {
        // previous
        moveToPreviousGlossItem(wdw);
    }
}

void InterlinearDisplayWidget::rightGlossItem( WordDisplayWidget * wdw )
{
    if( wdw->glossItem()->baselineWritingSystem().layoutDirection() == Qt::LeftToRight )
    {
        // previous
        moveToPreviousGlossItem(wdw);
    }
    else
    {
        // next
        moveToNextGlossItem(wdw);
    }
}

void InterlinearDisplayWidget::moveToNextGlossItem(WordDisplayWidget *wdw)
{
    int lineNumber = lineNumberOfWdw(wdw);
    if( lineNumber != -1 )
    {
        int position = mText->phrases()->at(lineNumber)->indexOfGlossItem(wdw->glossItem());
        if( position != -1 )
        {
            if( position == mWordDisplayWidgets.at(lineNumber).count() - 1 )
            {
                // move to first of next line
                if( lineNumber+1 < mWordDisplayWidgets.count() )
                {
                    mWordDisplayWidgets.at(lineNumber+1).first()->receiveKeyboardFocus();
                }
            }
            else
            {
                // move to position + 1
                mWordDisplayWidgets.at(lineNumber).at(position+1)->receiveKeyboardFocus();
            }
        }
    }
}

void InterlinearDisplayWidget::moveToPreviousGlossItem(WordDisplayWidget *wdw)
{
    int lineNumber = lineNumberOfWdw(wdw);
    if( lineNumber != -1 )
    {
        int position = mText->phrases()->at(lineNumber)->indexOfGlossItem(wdw->glossItem());
        if( position != -1 )
        {
            if( position == 0 )
            {
                // move to last of previous line
                if( lineNumber-1 >= 0 )
                {
                    mWordDisplayWidgets.at(lineNumber-1).last()->receiveKeyboardFocus();
                }
            }
            else
            {
                // move to position + 1
                mWordDisplayWidgets.at(lineNumber).at(position-1)->receiveKeyboardFocus();
            }
        }
    }
}
