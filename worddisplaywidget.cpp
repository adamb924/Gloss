#include "worddisplaywidget.h"
#include "textbit.h"
#include "project.h"
#include "lingedit.h"
#include "interlinearitemtype.h"
#include "databaseadapter.h"
#include "interlineardisplaywidget.h"

#include <QtGui>
#include <QtDebug>
#include <QActionGroup>

WordDisplayWidget::WordDisplayWidget( GlossItem *item, Qt::Alignment alignment, InterlinearDisplayWidget *ildw, DatabaseAdapter *dbAdapter)
{
    mDbAdapter = dbAdapter;
    mGlossItem = item;
    mAlignment = alignment;
    mInterlinearDisplayWidget = ildw;

    mGlossLines = mDbAdapter->interlinearTextLines();

    setupLayout();

    setMinimumSize(128,128);

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));

    connect( mGlossItem, SIGNAL(approvalStatusChanged(ApprovalStatus)), this, SLOT(updateBaselineLabelStyle()) );
    connect( mGlossItem, SIGNAL(candidateStatusChanged(CandidateStatus)), this, SLOT(updateBaselineLabelStyle()) );
    connect( mGlossItem, SIGNAL(fieldsChanged()), this, SLOT(fillData()) );

    fillData();

    updateBaselineLabelStyle();
}

void WordDisplayWidget::setupLayout()
{
    mLayout = new QVBoxLayout;
    setLayout(mLayout);

    mTextFormEdits.clear();
    mGlossEdits.clear();

    mBaselineWordLabel = new QLabel(mGlossItem->baselineText().text());
    mBaselineWordLabel->setAlignment( mGlossItem->writingSystem().layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight );
    updateBaselineLabelStyle();

    mLayout->addWidget(mBaselineWordLabel);
    for(int i=0; i<mGlossLines.count(); i++)
    {
        LingEdit *edit;
        if( mGlossLines.at(i).type() == InterlinearItemType::Text )
        {
            edit = addTextFormLine( mGlossLines.at(i) );
            mTextFormEdits.insert(mGlossLines.at(i).writingSystem(), edit);
        }
        else
        {
            edit = addGlossLine( mGlossLines.at(i) );
            mGlossEdits.insert(mGlossLines.at(i).writingSystem(), edit);
        }
        mLayout->addWidget(edit);
    }
}

LingEdit* WordDisplayWidget::addGlossLine( const InterlinearItemType & glossLine )
{
    LingEdit *edit = new LingEdit( mGlossItem->gloss( glossLine.writingSystem() ) , this);
    edit->setAlignment(calculateAlignment( mGlossItem->writingSystem().layoutDirection() ,glossLine.writingSystem().layoutDirection() ) );

    connect(this, SIGNAL(glossIdChanged(LingEdit*,qlonglong)), edit, SLOT(setId(LingEdit*,qlonglong)));
    connect(edit,SIGNAL(stringChanged(TextBit)), mGlossItem, SLOT(updateGloss(TextBit)) );
    connect(edit, SIGNAL(stringChanged(TextBit)), mInterlinearDisplayWidget, SLOT(updateGloss(TextBit)));

    return edit;
}

LingEdit* WordDisplayWidget::addTextFormLine( const InterlinearItemType & glossLine )
{
    LingEdit *edit = new LingEdit(  mGlossItem->textForm( glossLine.writingSystem() ) , this);
    edit->setAlignment(calculateAlignment( mGlossItem->writingSystem().layoutDirection() , glossLine.writingSystem().layoutDirection() ) );

    connect(this, SIGNAL(textFormIdChanged(LingEdit*,qlonglong)), edit, SLOT(setId(LingEdit*,qlonglong)));
    connect(edit,SIGNAL(stringChanged(TextBit)), mGlossItem, SLOT(updateText(TextBit)) );
    connect(edit, SIGNAL(stringChanged(TextBit)), mInterlinearDisplayWidget, SLOT(updateText(TextBit)));

    return edit;
}

void WordDisplayWidget::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    // Approved button
    QAction *approved = new QAction(tr("Approved"),&menu);
    approved->setCheckable(true);
    approved->setChecked(mGlossItem->approvalStatus() == GlossItem::Approved );
    QActionGroup *gApproved = new QActionGroup(&menu);
    gApproved->addAction(approved);
    menu.addAction(approved);
    connect( gApproved, SIGNAL(triggered(QAction*)) , mGlossItem, SLOT(toggleApproval()) );

    addInterpretationSubmenu(&menu);


    for(int i=0; i<mGlossLines.count(); i++)
    {
        if( mGlossLines.at(i).type() == InterlinearItemType::Gloss )
            addGlossSubmenu( &menu , mGlossLines.at(i).writingSystem() );
        else
            addTextFormSubmenu( &menu , mGlossLines.at(i).writingSystem() );
    }

    menu.exec(event->globalPos());
}

void WordDisplayWidget::addInterpretationSubmenu(QMenu *menu )
{
    QMenu *submenu = new QMenu(tr("Interpretation") ,menu);

    // Interpretation candidates
    QHash<qlonglong,QString> candidates = mDbAdapter->candidateInterpretationWithSummaries( mGlossItem->baselineText() );
    if(candidates.count() > 1)
    {
        QActionGroup *gCandidates = new QActionGroup(submenu);
        QHashIterator<qlonglong,QString> iter(candidates);
        while(iter.hasNext())
        {
            iter.next();
            QAction *candidate = new QAction( iter.value(), submenu );
            candidate->setCheckable(true);
            if( mGlossItem->id() == iter.key() )
                candidate->setChecked(true);
            candidate->setData(iter.key());
            gCandidates->addAction(candidate);
            submenu->addAction(candidate);
        }
        connect(gCandidates, SIGNAL(triggered(QAction*)), this, SLOT(selectDifferentCandidate(QAction*)));
        submenu->addSeparator();
    }

    submenu->addAction(tr("Alternate interpretation..."),this,SLOT(newInterpretation()));

    menu->addMenu(submenu);
}

void WordDisplayWidget::addTextFormSubmenu(QMenu *menu, const WritingSystem & writingSystem )
{
    QMenu *submenu = new QMenu(writingSystem.name() ,menu);

    QHash<qlonglong,QString> textForms = mDbAdapter->interpretationTextForms(mGlossItem->id(), writingSystem.id() );
    if(textForms.count() > 1 )
    {
        QActionGroup *gTextForms = new QActionGroup(submenu);
        QHashIterator<qlonglong,QString> iter(textForms);
        while(iter.hasNext())
        {
            iter.next();
            QAction *textForm = new QAction( iter.value(), menu );
            textForm->setCheckable(true);
            if( mGlossItem->textForms()->value(writingSystem).id() == iter.key() )
                textForm->setChecked(true);
            textForm->setData(iter.key());
            gTextForms->addAction(textForm);
            submenu->addAction(textForm);
        }
        connect( gTextForms , SIGNAL(triggered(QAction*)) , this , SLOT(selectDifferentTextForm(QAction*)) );
        submenu->addSeparator();
    }

    // Text forms
    QActionGroup *oneOffgroup = new QActionGroup(menu);
    QAction *action = new QAction(tr("Alternate text form..."),menu);
    action->setData( writingSystem.id() );
    oneOffgroup->addAction(action);
    connect( oneOffgroup, SIGNAL(triggered(QAction*)), this, SLOT(newTextForm(QAction*)) );
    submenu->addAction(action);

    menu->addMenu(submenu);
}

void WordDisplayWidget::addGlossSubmenu(QMenu *menu, const WritingSystem & writingSystem )
{
    QMenu *submenu = new QMenu(writingSystem.name() ,menu);

    QHash<qlonglong,QString> glosses = mDbAdapter->interpretationGlosses(mGlossItem->id(), writingSystem.id() );
    if( glosses.count() > 1 )
    {
        QActionGroup *gGlosses = new QActionGroup(submenu);
        QHashIterator<qlonglong,QString> iter(glosses);
        while(iter.hasNext())
        {
            iter.next();
            QAction *gloss = new QAction( iter.value(), menu );
            gloss->setCheckable(true);
            if( mGlossItem->glosses()->value(writingSystem).id() == iter.key() )
                gloss->setChecked(true);
            gloss->setData(iter.key());
            gGlosses->addAction(gloss);
            submenu->addAction(gloss);
        }
        connect( gGlosses, SIGNAL(triggered(QAction*)), this, SLOT(selectDifferentGloss(QAction*)) );
        submenu->addSeparator();
    }

    // Glosses
    QActionGroup *oneOffgroup = new QActionGroup(menu);
    QAction *action = new QAction(tr("Alternate gloss..."),menu);
    action->setData( writingSystem.id() );
    oneOffgroup->addAction(action);
    connect( oneOffgroup, SIGNAL(triggered(QAction*)), this, SLOT(newGloss(QAction*)) );
    submenu->addAction(action);

    menu->addMenu(submenu);
}

void WordDisplayWidget::newInterpretation()
{
    qlonglong id = mDbAdapter->newInterpretation( mGlossItem->baselineText() );
    mGlossItem->setInterpretation(id);
    mGlossItem->setCandidateStatus(GlossItem::MultipleOption);
    fillData();
}

void WordDisplayWidget::newGloss(QAction *action)
{
    qlonglong wsId = action->data().toLongLong();
    qlonglong id = mDbAdapter->newGloss( mGlossItem->id() , wsId );
    WritingSystem ws = mDbAdapter->writingSystem(wsId);
    mGlossItem->setGloss(TextBit("", ws , id ));
    emit glossIdChanged( mGlossEdits.value(ws) , id );
    fillData();
}

void WordDisplayWidget::newTextForm(QAction *action)
{
    qlonglong wsId = action->data().toLongLong();
    qlonglong id = mDbAdapter->newTextForm( mGlossItem->id() , wsId );
    WritingSystem ws = mDbAdapter->writingSystem(wsId);
    mGlossItem->setTextForm(TextBit("", ws, id ));
    emit textFormIdChanged( mTextFormEdits.value(ws) , id );
    fillData();
}

void WordDisplayWidget::fillData()
{
    if( mGlossItem->id() != -1 )
    {
        for(int i=0; i<mGlossLines.count();i++)
        {
            switch( mGlossLines.at(i).type() )
            {
            case InterlinearItemType::Text:
                mTextFormEdits[mGlossLines.at(i).writingSystem()]->setTextBit( mGlossItem->textForm(mGlossLines.at(i).writingSystem()) );
                break;
            case InterlinearItemType::Gloss:
                mGlossEdits[mGlossLines.at(i).writingSystem()]->setTextBit( mGlossItem->gloss( mGlossLines.at(i).writingSystem() ) );
                break;
            }
        }
    }
}

Qt::Alignment WordDisplayWidget::calculateAlignment( Qt::LayoutDirection match , Qt::LayoutDirection current ) const
{
    if( match == Qt::LeftToRight )
    {
        if( current == Qt::LeftToRight )
            return Qt::AlignLeft;
        else
            return Qt::AlignRight;
    }
    else // Qt::RightToLeft
    {
        if( current == Qt::LeftToRight )
            return Qt::AlignRight;
        else
            return Qt::AlignLeft;
    }
}

void WordDisplayWidget::updateBaselineLabelStyle()
{
    QString color;
    if ( mGlossItem->approvalStatus() == GlossItem::Unapproved && mGlossItem->candidateStatus() == GlossItem::SingleOption )
        color = "#CDFFB2";
    else if ( mGlossItem->approvalStatus() == GlossItem::Unapproved && mGlossItem->candidateStatus() == GlossItem::MultipleOption )
        color = "#fff6a8";
    else
        color = "#ffffff";
    mBaselineWordLabel->setStyleSheet(QString("font-family: %1; font-size: %2pt; background-color: %3;").arg(mGlossItem->baselineText().writingSystem().fontFamily()).arg(mGlossItem->baselineText().writingSystem().fontSize()).arg(color));
}

void WordDisplayWidget::mouseDoubleClickEvent ( QMouseEvent * event )
{
    mGlossItem->toggleApproval();
}

void WordDisplayWidget::selectDifferentCandidate(QAction *action)
{
    mGlossItem->setInterpretation( action->data().toLongLong() );
}

void WordDisplayWidget::selectDifferentGloss(QAction *action)
{
    TextBit bit = mDbAdapter->glossFromId( action->data().toLongLong() );
    mGlossItem->setGloss( bit );
    emit glossIdChanged( mGlossEdits.value( bit.writingSystem() ) , bit.id() );
}

void WordDisplayWidget::selectDifferentTextForm(QAction *action)
{
    TextBit bit = mDbAdapter->textFormFromId(action->data().toLongLong());
    mGlossItem->setTextForm( bit );
    emit textFormIdChanged( mTextFormEdits.value(bit.writingSystem()), bit.id());
}

QHash<qlonglong, LingEdit*> WordDisplayWidget::textFormEdits() const
{
    QHash<qlonglong, LingEdit*> hash;
    QHashIterator<WritingSystem, LingEdit*> iter(mTextFormEdits);
    while( iter.hasNext() )
    {
        iter.next();
        hash.insert( iter.value()->id() , iter.value() );
    }
    return hash;
}

QHash<qlonglong, LingEdit*> WordDisplayWidget::glossEdits() const
{
    QHash<qlonglong, LingEdit*> hash;
    QHashIterator<WritingSystem, LingEdit*> iter(mGlossEdits);
    while( iter.hasNext() )
    {
        iter.next();
        hash.insert( iter.value()->id() , iter.value() );
    }
    return hash;
}

void WordDisplayWidget::sendConcordanceUpdates()
{
    QHashIterator<WritingSystem, LingEdit*> iter(mTextFormEdits);
    while(iter.hasNext())
    {
        iter.next();
        WritingSystem ws = iter.key();
        emit textFormIdChanged( iter.value() , mGlossItem->textForm( ws ).id() );
    }

    iter = QHashIterator<WritingSystem, LingEdit*>(mGlossEdits);
    while(iter.hasNext())
    {
        iter.next();
        emit glossIdChanged( iter.value() , mGlossItem->gloss( iter.key() ).id() );
    }
}
