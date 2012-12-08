#include "worddisplaywidget.h"
#include "textbit.h"
#include "project.h"
#include "lingedit.h"
#include "glossline.h"
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

    mGlossLines = mDbAdapter->glossLines();

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

    mEdits.clear();

    mBaselineWordLabel = new QLabel(mGlossItem->baselineText().text());
    mBaselineWordLabel->setAlignment( mGlossItem->writingSystem().layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight );
    updateBaselineLabelStyle();

    mLayout->addWidget(mBaselineWordLabel);
    for(int i=0; i<mGlossLines.count(); i++)
    {
        LingEdit *edit;
        if( mGlossLines.at(i).type() == GlossLine::Text )
            edit = addTextFormLine( mGlossLines.at(i) );
        else
            edit = addGlossLine( mGlossLines.at(i) );
        mLayout->addWidget(edit);
        mEdits.insert(mGlossLines.at(i).writingSystem(), edit);
    }
}

LingEdit* WordDisplayWidget::addGlossLine( const GlossLine & glossLine )
{
    LingEdit *edit = new LingEdit( mGlossItem->glosses()->value( glossLine.writingSystem() ) , this);
    edit->setAlignment(calculateAlignment( mGlossItem->writingSystem().layoutDirection() ,glossLine.writingSystem().layoutDirection() ) );

    connect( mGlossItem, SIGNAL(glossIdChanged(qlonglong)), edit, SLOT(changeId(qlonglong)));
    connect(edit, SIGNAL(stringChanged(TextBit)), mInterlinearDisplayWidget, SLOT(updateGloss(TextBit)));
    connect(edit,SIGNAL(stringChanged(TextBit)), mGlossItem, SLOT(updateGloss(TextBit)) );

    return edit;
}

LingEdit* WordDisplayWidget::addTextFormLine( const GlossLine & glossLine )
{
    // this is right on the edge of being better handled with two methods
    LingEdit *edit = new LingEdit(  mGlossItem->textForms()->value( glossLine.writingSystem() ) , this);
    edit->setAlignment(calculateAlignment( mGlossItem->writingSystem().layoutDirection() , glossLine.writingSystem().layoutDirection() ) );

    connect( mGlossItem, SIGNAL(textFormIdChanged(qlonglong)), edit, SLOT(changeId(qlonglong)));
    connect(edit, SIGNAL(stringChanged(TextBit)), mInterlinearDisplayWidget, SLOT(updateText(TextBit)));
    connect(edit,SIGNAL(stringChanged(TextBit)), mGlossItem, SLOT(updateText(TextBit)) );

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
        if( mGlossLines.at(i).type() == GlossLine::Gloss )
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
    mGlossItem->setGloss(TextBit("", mDbAdapter->writingSystem(wsId) , id ));
    fillData();
}

void WordDisplayWidget::newTextForm(QAction *action)
{
    qlonglong wsId = action->data().toLongLong();
    qlonglong id = mDbAdapter->newTextForm( mGlossItem->id() , wsId );
    mGlossItem->setTextForm(TextBit("", mDbAdapter->writingSystem(wsId), id ));
    fillData();
}

void WordDisplayWidget::fillData()
{
    if( mGlossItem->id() != -1 )
    {
        for(int i=0; i<mGlossLines.count();i++)
        {
            QString form;
            switch( mGlossLines.at(i).type() )
            {
            case GlossLine::Text:
                form = mGlossItem->textForms()->value( mGlossLines.at(i).writingSystem() ).text();
                break;
            case GlossLine::Gloss:
                form = mGlossItem->glosses()->value( mGlossLines.at(i).writingSystem() ).text();
                break;
            }
            mEdits[mGlossLines.at(i).writingSystem()]->setText( form );
        }
    }
}

void WordDisplayWidget::updateEdit( const TextBit & bit, GlossLine::LineType type )
{
    LingEdit *line = mEdits.value(bit.writingSystem());
    if( line != 0 )
        line->setText(bit.text());
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
        color = "#9aff67";
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
    mGlossItem->setGloss( mDbAdapter->glossFromId( action->data().toLongLong() ) );
}

void WordDisplayWidget::selectDifferentTextForm(QAction *action)
{
    mGlossItem->setTextForm( mDbAdapter->textFormFromId(action->data().toLongLong()) );
}
