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
#include "immutablelabel.h"
#include "punctuationdisplaywidget.h"
#include "paragraph.h"
#include "paragraphmarkwidget.h"
#include "mainwindow.h"

InterlinearDisplayWidget::InterlinearDisplayWidget(const Tab * tab, Text *text, Project *project, QWidget *parent) :
    QScrollArea(parent), mTab(tab), mText(text), mProject(project), mMouseMode(InterlinearDisplayWidget::Normal)
{
    mLines.clear();

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

void InterlinearDisplayWidget::addLineLabel( int i , Phrase * phrase, QLayout * flowLayout  )
{
    InterlinearLineLabel *lineNumber = new InterlinearLineLabel(mText, phrase, i, this);
    flowLayout->addWidget(lineNumber);
    mLineLabels.insert(i, lineNumber);
}

QLayout* InterlinearDisplayWidget::addLine(int lineNumber)
{
    InterlinearItemTypeList * lines = mTab->interlinearLines(mText->baselineWritingSystem());
    if( lines->isEmpty() )
    {
        return new FlowLayout( Qt::LeftToRight, nullptr, 5 , 5 , 5 );
    }
    FlowLayout *flowLayout = new FlowLayout( lines->first().writingSystem().layoutDirection() , nullptr, 5 , 5 , 5 );

    mLineLayouts.insert(lineNumber, flowLayout);
    mLayout->addLayout(flowLayout);
    return flowLayout;
}

QVBoxLayout* InterlinearDisplayWidget::addPhrasalGlossLayout(int lineNumber)
{
    QVBoxLayout *layout = new QVBoxLayout;
    mPhrasalGlossLayouts.insert(lineNumber, layout);
    mLayout->addLayout(layout);
    return layout;
}

void InterlinearDisplayWidget::saveText()
{
    mText->saveText(false, true, true);
}

void InterlinearDisplayWidget::saveTextVerbose()
{
    mText->saveText(true, true, true);
}

void InterlinearDisplayWidget::resetGui()
{
    QList<int> newLines = mLines;
    for(int i=0; i<newLines.count(); i++)
    {
        if( newLines.at(i) >= mText->phraseCount() )
        {
            newLines.removeAt(i);
        }
    }
    setLines(newLines);
}

void InterlinearDisplayWidget::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    menu.addAction(tr("Clear highlighting"), this, SLOT(clearFocus()), QKeySequence("Ctrl+Shift+C"));
    menu.addSeparator();
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
    if( MainWindow::mShortcuts.keysequence("NormalMouseMode").matches( toKeySequence(event) ) )
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
        mText->phraseAtLine(lineNumber)->replaceGlossItem(wdw->glossItem(), newBaseline);
    }
}

void InterlinearDisplayWidget::clearFocus()
{
    setFocus( QList<Focus>() );
}

void InterlinearDisplayWidget::scrollToLine(int line)
{
    if( mLineLayouts.value(line) != nullptr )
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

void InterlinearDisplayWidget::addPhrasalGlossLines( int i, Phrase *phrase, QVBoxLayout * phrasalGlossLayout )
{
    for(int j=0; j< mTab->phrasalGlossLines()->count(); j++)
    {
        TextBit bit = phrase->gloss( mTab->phrasalGlossLines()->at(j).writingSystem() );

        LingEdit *edit = new LingEdit( bit , this);
        phrasalGlossLayout->addWidget(edit);

        InterlinearItemTypeList * lines = mTab->interlinearLines(mText->baselineWritingSystem());
        if( lines->isEmpty() ) continue;

        edit->matchTextAlignmentTo( lines->first().writingSystem().layoutDirection() );
        connect( edit, SIGNAL(stringChanged(TextBit,LingEdit*)), phrase, SLOT(setPhrasalGloss(TextBit)) );

        mPhrasalGlossEdits.insert( i , edit );
        phrasalGlossLayout->addWidget( edit );
    }
}

void InterlinearDisplayWidget::addParagraphMarker(int lineIndex, Paragraph *paragraph, QLayout *before)
{
    ParagraphMarkWidget * mark = new ParagraphMarkWidget(paragraph,this);
    if( before == nullptr ) /// add it to the end
    {
        mLayout->addWidget( mark );
    }
    else /// find \a before and insert it before that
    {
        for(int i=0; i<mLayout->count(); i++)
        {
            if( mLayout->itemAt(i)->layout() == before )
            {
                mLayout->insertWidget(i, mark );
                break;
            }
        }
    }
    mParagraphMarkWidgets.insert(lineIndex, mark);
    connect( mark, SIGNAL(removeParagraphDivision(Paragraph*)), mText, SLOT(removeParagraphDivision(Paragraph*)) );
}

void InterlinearDisplayWidget::clearWidgetsFromLine(int lineNumber)
{
    QLayout *layout = mLineLayouts.value(lineNumber);
    if( layout != nullptr )
    {
        QWidget *lineLabel = mLineLabels.take(lineNumber);
        if( lineLabel != nullptr )
        {
            lineLabel->deleteLater();
            layout->removeWidget(lineLabel);
        }

        QListIterator<QWidget*> iter( mWordDisplayWidgets.at(lineNumber) );
        while(iter.hasNext())
        {
            QWidget *wdw = iter.next();
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

        QListIterator<ParagraphMarkWidget*> paraIter( mParagraphMarkWidgets.values(lineNumber) );
        while(paraIter.hasNext())
        {
            ParagraphMarkWidget *marker = paraIter.next();
            layout->removeWidget(marker);
            marker->deleteLater();
            mParagraphMarkWidgets.remove( lineNumber, marker );
        }
   }
}

void InterlinearDisplayWidget::setLayoutFromText()
{
    if( mLines.isEmpty() )
        setLinesToDefault();

    QProgressDialog progress(tr("Creating interface for %1...").arg(mText->name()), "Cancel", 0, mLines.count(), nullptr);
    progress.setWindowModality(Qt::WindowModal);

    int nPhrases = mText->phraseCount();

    QListIterator<int> iter(mLines);
    while(iter.hasNext())
    {
        progress.setValue( progress.value() + 1 );

        int lineIndex = iter.next();

        // bit of a kludge, but saves a crash
        if( lineIndex >= nPhrases )
        {
            continue;
        }

        QLayout *flowLayout;
        QVBoxLayout *phrasalGlossLayout;

        Phrase * phrase = mText->phraseAtLine(lineIndex);
        Paragraph * paragraph = mText->paragraphForPhrase(phrase);
        /// add a paragraph marker if there is more than one paragraph in the text && this is the first phrase in the paragraph
        bool paragraphMarkerAppropriate = mText->paragraphs()->count() > 1 && paragraph->indexOf(phrase) == 0;

        if( !paragraphMarkerAppropriate && mParagraphMarkWidgets.contains(lineIndex) ) /// there should then be no paragraph marker. if one exists, remove it
        {
            ParagraphMarkWidget * marker = mParagraphMarkWidgets.value(lineIndex);
            mLayout->removeWidget( marker );
            marker->deleteLater();
            mParagraphMarkWidgets.remove( lineIndex, marker );
        }

        if( mLineLayouts.value(lineIndex) == nullptr ) /// if there is no layout for this line, a new layout needs to be created
        {
            if( paragraphMarkerAppropriate )
            {
                addParagraphMarker(lineIndex, paragraph);
            }
            flowLayout = addLine(lineIndex); /// the flow layout is added to mLayout here
            phrasalGlossLayout = addPhrasalGlossLayout(lineIndex);
        }
        else if( mLineRefreshRequests.contains( lineIndex ) ) /// if the program has requested the line to be redrawn, the widgets in the layout should be cleared away
        {
            flowLayout = mLineLayouts.value(lineIndex);
            phrasalGlossLayout = mPhrasalGlossLayouts.value(lineIndex);
            clearWidgetsFromLine(lineIndex);
            if( paragraphMarkerAppropriate && !mParagraphMarkWidgets.contains(lineIndex) )
            {
                addParagraphMarker(lineIndex, paragraph, flowLayout);
            }
        }
        else /// there is a layout for this line, and no refresh has been requested, so move on
        {
            continue;
        }

        if( flowLayout->isEmpty() )
        {
            addLineLabel(lineIndex, phrase, flowLayout);
            addWordWidgets(lineIndex, phrase, flowLayout);
            addPhrasalGlossLines(lineIndex, phrase, phrasalGlossLayout);
        }

        mLineRefreshRequests.clear();
    }
    progress.setValue(mLines.count());

    mLayout->addStretch(1);
}

void InterlinearDisplayWidget::addWordWidgets( int i , Phrase * phrase, QLayout * flowLayout )
{
    for(int j=0; j<phrase->glossItemCount(); j++)
    {
        QWidget *wdw;
        if( phrase->glossItemAt(j)->isPunctuation() )
        {
            wdw = addPunctuationDisplayWidget( phrase->glossItemAt(j) );
        }
        else
        {
            wdw = addWordDisplayWidget( phrase->glossItemAt(j), phrase);
        }

        /// add another list to mWordDisplayWidgets if necessary
        while( i >= mWordDisplayWidgets.count() )
            mWordDisplayWidgets.append( QList<QWidget*>() );
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

    connect( wdw, SIGNAL(requestApprovePhrase(WordDisplayWidget*)), this, SLOT(approveAll(WordDisplayWidget*)) );
    connect( wdw, SIGNAL(requestPlaySound(WordDisplayWidget*)), this, SLOT(playSound(WordDisplayWidget*)) );
    connect( wdw, SIGNAL(requestLeftGlossItem(WordDisplayWidget*)), this, SLOT(leftGlossItem(WordDisplayWidget*)));
    connect( wdw, SIGNAL(requestRightGlossItem(WordDisplayWidget*)), this, SLOT(rightGlossItem(WordDisplayWidget*)));

    connect( wdw, SIGNAL(requestSetFollowingInterpretations(GlossItem*)), mText, SLOT(setFollowingInterpretations(GlossItem*)) );
    connect( wdw, SIGNAL(requestReplaceFollowing(GlossItem*,QString)), mText, SLOT(replaceFollowing(GlossItem*,QString)) );
    connect( wdw, SIGNAL(requestSetFollowingTextForms(GlossItem*,WritingSystem)), mText, SLOT(matchFollowingTextForms(GlossItem*,WritingSystem)) );
    connect( wdw, SIGNAL(requestSetFollowingGlosses(GlossItem*,WritingSystem)), mText, SLOT(matchFollowingGlosses(GlossItem*,WritingSystem)) );

    connect( wdw, SIGNAL(requestNewPhraseFromHere(GlossItem*)), mText, SLOT(newPhraseStartingHere(GlossItem*)) );
    connect( wdw, SIGNAL(requestNoPhraseFromHere(GlossItem*)), mText, SLOT(noNewPhraseStartingHere(GlossItem*)) );

    connect( wdw, SIGNAL(leftClicked(WordDisplayWidget*)), this, SLOT(wdwClicked(WordDisplayWidget*)) );

    return wdw;
}

PunctuationDisplayWidget *InterlinearDisplayWidget::addPunctuationDisplayWidget(GlossItem *item)
{
    PunctuationDisplayWidget *pdw = new PunctuationDisplayWidget( item, mProject->dbAdapter() , this );
    connect( pdw, SIGNAL(requestNewPhraseFromHere(GlossItem*)), mText, SLOT(newPhraseStartingHere(GlossItem*)) );
    connect( pdw, SIGNAL(requestNoPhraseFromHere(GlossItem*)), mText, SLOT(noNewPhraseStartingHere(GlossItem*)) );
    return pdw;
}

bool InterlinearDisplayWidget::maybeFocus(QWidget * wdw)
{
    WordDisplayWidget * wdwRecast = qobject_cast<WordDisplayWidget*>(wdw);
    if( wdwRecast == nullptr  )
    {
        return false;
    }
    bool isFocused = false;
    for(int i=0; i<mFoci.count(); i++)
    {
        if( wdwRecast->glossItem()->matchesFocus( mFoci.at(i) ) )
        {
            isFocused = true;
            break;
        }
    }
    wdwRecast->setFocused(isFocused);
    return isFocused;
}

void InterlinearDisplayWidget::setLinesToDefault()
{
    mLines.clear();
    for(int i=0; i<mText->phraseCount(); i++)
        mLines << i;
}

void InterlinearDisplayWidget::setLines( const QList<int> lines )
{
    QList<int> currentLines = mLineLayouts.keys();
    for(int i=0; i<currentLines.count(); i++)
    {
        clearWidgetsFromLine(currentLines.at(i));
    }
    mLines = lines;
    qDeleteAll(mLineLayouts);
    mLineLayouts.clear();
    qDeleteAll(mPhrasalGlossLayouts);
    mPhrasalGlossLayouts.clear();
    mLineLabels.clear();

    // http://stackoverflow.com/questions/4272196/qt-remove-all-widgets-from-layout
    QLayoutItem *wItem;
    while( (wItem = mLayout->takeAt(0)) != nullptr)
    {
          delete wItem;
    }
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

    bool firstTime = true;

    for( int i=0; i<mWordDisplayWidgets.count(); i++ )
    {
        for( int j=0; j<mWordDisplayWidgets.at(i).count(); j++)
        {
            if( maybeFocus( mWordDisplayWidgets.at(i).at(j) ) )
            {
                if(firstTime)
                {
                    firstTime = false;
                    qApp->processEvents();
                    ensureWidgetVisible( mWordDisplayWidgets.at(i).at(j), 250 , 250 );
                }
            }
        }
    }
}

void InterlinearDisplayWidget::approveAll( WordDisplayWidget * wdw )
{
    int lineNumber = lineNumberOfWdw(wdw);
    if( lineNumber != -1 )
    {
        mText->phraseAtLine(lineNumber)->setApproval(GlossItem::Approved);
    }
}

void InterlinearDisplayWidget::playSound( WordDisplayWidget * wdw )
{
    int lineNumber = lineNumberOfWdw(wdw);
    if( lineNumber != -1 )
    {
        mText->playSoundForLine(lineNumber);
    }
}

int InterlinearDisplayWidget::lineNumberOfWdw(QWidget *wdw ) const
{
    if( qobject_cast<WordDisplayWidget*>(wdw) == nullptr  )
    {
        return -1;
    }
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
        int position = mText->phraseAtLine(lineNumber)->indexOfGlossItem(wdw->glossItem());
        if( position != -1 )
        {
            findNextWdw(lineNumber,position);
            qobject_cast<WordDisplayWidget*>(mWordDisplayWidgets.at(lineNumber).at(position))->receiveKeyboardFocus();
        }
    }
}

void InterlinearDisplayWidget::moveToPreviousGlossItem(WordDisplayWidget *wdw)
{
    int lineNumber = lineNumberOfWdw(wdw);
    if( lineNumber != -1 )
    {

        int position = mText->phraseAtLine(lineNumber)->indexOfGlossItem(wdw->glossItem());
        if( position != -1 )
        {
            findPreviousWdw(lineNumber,position);
            qobject_cast<WordDisplayWidget*>(mWordDisplayWidgets.at(lineNumber).at(position))->receiveKeyboardFocus();
        }
    }
}

void InterlinearDisplayWidget::findPreviousWdw(int &lineNumber, int &position)
{
    do
    {
        if( position == 0 )
        {
            // move to last of previous line
            if( lineNumber-1 >= 0 )
            {
                lineNumber--;
                position = mWordDisplayWidgets.at(lineNumber).count() - 1;
            }
        }
        else
        {
            position--;
        }
    } while( qobject_cast<WordDisplayWidget*>(mWordDisplayWidgets.at(lineNumber).at(position)) == nullptr  && lineNumber >= 0 && position >= 0 );
}

void InterlinearDisplayWidget::findNextWdw(int &lineNumber, int &position)
{
    do
    {
        if( position == mWordDisplayWidgets.at(lineNumber).count() - 1 )
        {
            // move to first of next line
            if( lineNumber+1 < mWordDisplayWidgets.count() )
            {
                lineNumber++;
                position = 0;
            }
        }
        else
        {
            position++;
        }
    } while( qobject_cast<WordDisplayWidget*>(mWordDisplayWidgets.at(lineNumber).at(position)) == nullptr && lineNumber < mWordDisplayWidgets.count() && position < mWordDisplayWidgets.at(lineNumber).count() );
}
