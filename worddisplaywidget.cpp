#include "worddisplaywidget.h"
#include "textbit.h"
#include "project.h"
#include "lingedit.h"
#include "interlinearitemtype.h"
#include "databaseadapter.h"
#include "interlineardisplaywidget.h"
#include "immutablelabel.h"
#include "analysiswidget.h"
#include "generictextinputdialog.h"
#include "mainwindow.h"
#include "dealwithspacesdialog.h"
#include "annotationmarkwidget.h"

#include <QtWidgets>
#include <QtDebug>
#include <QActionGroup>

WordDisplayWidget::WordDisplayWidget(GlossItem *item, Qt::Alignment alignment, const Tab * tab, const Project * project, QWidget *parent) :
    QFrame(parent),
    mProject(project),
    mTab(tab),
    mDbAdapter(project->dbAdapter()),
    mGlossItem(item),
    mConcordance(mGlossItem->concordance()),
    mAlignment(alignment)
{
    setStyleSheet("WordDisplayWidget { border: 1px solid transparent; background-color: transparent; }");

    setObjectName("WordDisplayWidget");

    mGlossLines = *mTab->interlinearLines().value( mGlossItem->baselineWritingSystem() );

    setupLayout();

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));

    connect( mGlossItem, SIGNAL(fieldsChanged()), this, SLOT(fillData()), Qt::UniqueConnection );
    connect( this, SIGNAL(requestInterpretationSearch(qlonglong)), mGlossItem->project()->mainWindow(), SLOT( searchForInterpretationById(qlonglong) ));
    connect( this, SIGNAL(requestTextFormSearch(qlonglong)), mGlossItem->project()->mainWindow(), SLOT( searchForTextFormById(qlonglong) ));
    connect( this, SIGNAL(requestGlossSearch(qlonglong)), mGlossItem->project()->mainWindow(), SLOT( searchForGlossById(qlonglong) ));

    setupShortcuts();

    fillData();
}

WordDisplayWidget::~WordDisplayWidget()
{
}

void WordDisplayWidget::setupLayout()
{
    QHBoxLayout *hLayout = new QHBoxLayout;

    mAnnotationMarks = new AnnotationMarkWidget( *(mProject->annotationTypes()) , mGlossItem );
    connect( mAnnotationMarks, SIGNAL(annotationActivated(QString)), this, SLOT(annotationMarkActivated(QString)) );

    QVBoxLayout *vLayout = new QVBoxLayout;

    if( mGlossLines.first().writingSystem().layoutDirection() == Qt::LeftToRight )
    {
        hLayout->addWidget(mAnnotationMarks);
        hLayout->addLayout(vLayout);
    }
    else
    {
        hLayout->addLayout(vLayout);
        hLayout->addWidget(mAnnotationMarks);
    }

    setLayout(hLayout);

    mTextFormEdits.clear();
    mGlossEdits.clear();
    mImmutableLines.clear();
    mAnalysisWidgets.clear();

    QLabel *immutableLabel;
    LingEdit *lineWidget;
    AnalysisWidget *analysisWidget;

    for(int i=0; i<mGlossLines.count(); i++)
    {
        switch( mGlossLines.at(i).type() )
        {
        case InterlinearItemType::ImmutableText:
            immutableLabel = addImmutableTextFormLine( mGlossLines.at(i) , i == 0 );
            vLayout->addWidget(immutableLabel);
            break;
        case InterlinearItemType::ImmutableGloss:
            immutableLabel = addImmutableGlossLine( mGlossLines.at(i) , i == 0 );
            vLayout->addWidget(immutableLabel);
            break;
        case InterlinearItemType::Text:
            lineWidget = addTextFormLine( mGlossLines.at(i) );
            vLayout->addWidget(lineWidget);
            break;
        case InterlinearItemType::Gloss:
            lineWidget = addGlossLine( mGlossLines.at(i) );
            vLayout->addWidget(lineWidget);
            break;
        case InterlinearItemType::Analysis:
            analysisWidget = addAnalysisWidget( mGlossLines.at(i) );
            vLayout->addWidget(analysisWidget);
            break;
        case InterlinearItemType::Null:
            break;
        }
    }
}

void WordDisplayWidget::setupShortcuts()
{
    QAction *copyBaseline = new QAction(this);
    copyBaseline->setShortcut( QKeySequence("Ctrl+Shift+C") );
    copyBaseline->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect( copyBaseline, SIGNAL(triggered()), this, SLOT(copyGlossFromBaseline()) );
    addAction(copyBaseline);

    QAction *guessGloss = new QAction(this);
    guessGloss->setShortcut( QKeySequence("Ctrl+Shift+G") );
    guessGloss->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect( guessGloss, SIGNAL(triggered()), this, SLOT(guessGloss()) );
    addAction(guessGloss);

    QAction *editBaseline = new QAction(this);
    editBaseline->setShortcut( QKeySequence("Ctrl+Shift+B") );
    editBaseline->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect( editBaseline, SIGNAL(triggered()), this, SLOT(editBaselineText()) );
    addAction(editBaseline);

    QAction *approveLine = new QAction(this);
    approveLine->setShortcut( QKeySequence("Ctrl+Shift+A") );
    approveLine->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect( approveLine, SIGNAL(triggered()), this, SLOT(approveLine()) );
    addAction(approveLine);

    QAction *rightGlossItem = new QAction(this);
    rightGlossItem->setShortcut( QKeySequence("Alt+Right") );
    rightGlossItem->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect( rightGlossItem, SIGNAL(triggered()), this, SLOT(rightGlossItem()) );
    addAction(rightGlossItem);

    QAction *leftGlossItem = new QAction(this);
    leftGlossItem->setShortcut( QKeySequence("Alt+Left") );
    leftGlossItem->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect( leftGlossItem, SIGNAL(triggered()), this, SLOT(leftGlossItem()) );
    addAction(leftGlossItem);

    QAction *playSound = new QAction(this);
    playSound->setShortcut( QKeySequence("Ctrl+Shift+Q") );
    playSound->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect( playSound, SIGNAL(triggered()), this, SLOT(playSound()) );
    addAction(playSound);
}

LingEdit* WordDisplayWidget::addGlossLine( const InterlinearItemType & glossLine )
{
    TextBit gloss = mGlossItem->gloss( glossLine.writingSystem() );
    LingEdit *edit = new LingEdit( gloss , this);
    edit->matchTextAlignmentTo( mGlossLines.first().writingSystem().layoutDirection() );
    edit->setMaximumWidth(100);

    mGlossEdits.insert(glossLine.writingSystem(), edit);

    // update the gloss item
    connect(edit,SIGNAL(stringChanged(TextBit,LingEdit*)), mGlossItem, SLOT(setGloss(TextBit)) );

    return edit;
}

LingEdit* WordDisplayWidget::addTextFormLine( const InterlinearItemType & glossLine )
{
    TextBit textForm = mGlossItem->textForm( glossLine.writingSystem() );
    LingEdit *edit = new LingEdit(  textForm , this);
    edit->matchTextAlignmentTo( mGlossLines.first().writingSystem().layoutDirection() );
    edit->setMaximumWidth(100);

    mTextFormEdits.insert(glossLine.writingSystem(), edit);

    // update the gloss item
    connect(edit,SIGNAL(stringChanged(TextBit,LingEdit*)), mGlossItem, SLOT(setTextForm(TextBit)) );

    return edit;
}

ImmutableLabel* WordDisplayWidget::addImmutableTextFormLine( const InterlinearItemType & glossLine, bool technicolor )
{
    TextBit bit = mGlossItem->textForm( glossLine.writingSystem() );

    ImmutableLabel *immutableLabel = new ImmutableLabel( bit, technicolor , this);
    immutableLabel->setCandidateNumber(mGlossItem->candidateNumber());
    immutableLabel->setApprovalStatus(mGlossItem->approvalStatus());

    mImmutableLines.insert( glossLine.writingSystem() , immutableLabel );

    connect( mGlossItem, SIGNAL(approvalStatusChanged(GlossItem::ApprovalStatus)), immutableLabel, SLOT(setApprovalStatus(GlossItem::ApprovalStatus)) );
    connect( mGlossItem, SIGNAL(candidateNumberChanged(GlossItem::CandidateNumber)), immutableLabel, SLOT(setCandidateNumber(GlossItem::CandidateNumber)) );

    return immutableLabel;
}

ImmutableLabel* WordDisplayWidget::addImmutableGlossLine( const InterlinearItemType & glossLine, bool technicolor )
{
    TextBit bit = mGlossItem->gloss( glossLine.writingSystem() );

    ImmutableLabel *immutableLabel = new ImmutableLabel( bit, technicolor , this);
    immutableLabel->setAlignment( mGlossLines.first().writingSystem().layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight );
    immutableLabel->setCandidateNumber(mGlossItem->candidateNumber());
    immutableLabel->setApprovalStatus(mGlossItem->approvalStatus());

    mImmutableLines.insert( glossLine.writingSystem() , immutableLabel );

    connect( mGlossItem, SIGNAL(approvalStatusChanged(GlossItem::ApprovalStatus)), immutableLabel, SLOT(setApprovalStatus(GlossItem::ApprovalStatus)) );
    connect( mGlossItem, SIGNAL(candidateNumberChanged(GlossItem::CandidateNumber)), immutableLabel, SLOT(setCandidateNumber(GlossItem::CandidateNumber)) );

    return immutableLabel;
}

AnalysisWidget* WordDisplayWidget::addAnalysisWidget( const InterlinearItemType & glossLine )
{
    AnalysisWidget *analysisWidget = new AnalysisWidget(mGlossItem, glossLine.writingSystem(), mProject, this);
    mAnalysisWidgets.insert( glossLine.writingSystem(), analysisWidget );

    connect( mGlossItem, SIGNAL(morphologicalAnalysisChanged(MorphologicalAnalysis*)), analysisWidget, SLOT(setupLayout()));
    connect( analysisWidget, SIGNAL(morphologicalAnalysisChanged(MorphologicalAnalysis*)), mConcordance, SLOT(updateGlossItemMorphologicalAnalysis(MorphologicalAnalysis*)) );
    connect( analysisWidget, SIGNAL(requestAlternateInterpretation()), this, SLOT(duplicateInterpretation()) );

    return analysisWidget;
}

void WordDisplayWidget::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu *menu = new QMenu(this);

    menu->addAction(tr("Edit baseline text"), this, SLOT(editBaselineText()));
    menu->addAction(tr("Edit baseline text, keep annotations"), this, SLOT(editBaselineTextKeepAnnotations()));
    menu->addAction(tr("Match following to this interpretation"), this, SLOT(matchFollowingTextFormsToThis()));
    menu->addAction(tr("Edit baseline text of matching following"), this, SLOT(editBaselineTextMatchingFollowing()));

    menu->addAction(tr("Edit Text Form %1 (%2)").arg( mGlossItem->baselineText().id() ).arg( mGlossItem->baselineText().text() ), this, SLOT(editBaselineTextForm()));

    menu->addSeparator();
    menu->addAction(tr("Merge with next"), this, SLOT(mergeWithNext()));
    menu->addAction(tr("Merge with previous"), this, SLOT(mergeWithPrevious()));
    menu->addAction(tr("Remove"), this, SLOT(removeGlossItem()));

    menu->addSeparator();

    // Approved button
    QAction *approved = new QAction(tr("Approved"),menu);
    approved->setCheckable(true);
    approved->setChecked(mGlossItem->approvalStatus() == GlossItem::Approved );
    QActionGroup *gApproved = new QActionGroup(menu);
    gApproved->addAction(approved);
    menu->addAction(approved);
    connect( gApproved, SIGNAL(triggered(QAction*)) , mGlossItem, SLOT(toggleApproval()) );

    addInterpretationSubmenu(menu);


    for(int i=0; i<mGlossLines.count(); i++)
    {
        switch( mGlossLines.at(i).type() )
        {
        case InterlinearItemType::Text:
        case InterlinearItemType::ImmutableText:
            addTextFormSubmenu( menu , mGlossLines.at(i).writingSystem() );
            break;
        case InterlinearItemType::Gloss:
        case InterlinearItemType::ImmutableGloss:
            addGlossSubmenu( menu , mGlossLines.at(i).writingSystem() );
            break;
        case InterlinearItemType::Analysis:
        case InterlinearItemType::Null:
            break;
        }
    }

    menu->addSeparator();

    menu->addAction(tr("Database report"), this, SLOT(displayDatabaseReport()) );
    addSearchSubmenu(menu);

    menu->exec(event->globalPos());
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

    submenu->addAction(tr("New interpretation..."),this,SLOT(newInterpretation()));
    submenu->addAction(tr("Duplicate interpretation"),this,SLOT(duplicateInterpretation()));

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

        QActionGroup *oneOffgroup = new QActionGroup(menu);
        QAction *action = new QAction(tr("Match following text forms to this"),menu);
        action->setData( writingSystem.id() );
        oneOffgroup->addAction(action);
        connect( oneOffgroup, SIGNAL(triggered(QAction*)), this, SLOT(changeFollowingToMatchTextForm(QAction*)) );
        submenu->addAction(action);
    }

    // Text forms
    QActionGroup *oneOffgroup = new QActionGroup(menu);
    QAction *action = new QAction(tr("New text form..."),menu);
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

        QActionGroup *oneOffgroup = new QActionGroup(menu);
        QAction *action = new QAction(tr("Match following glosses to this"),menu);
        action->setData( writingSystem.id() );
        oneOffgroup->addAction(action);
        connect( oneOffgroup, SIGNAL(triggered(QAction*)), this, SLOT(changeFollowingToMatchGloss(QAction*)) );
        submenu->addAction(action);
    }

    // Glosses
    QActionGroup *oneOffgroup = new QActionGroup(menu);
    QAction *action = new QAction(tr("New gloss..."),menu);
    action->setData( writingSystem.id() );
    oneOffgroup->addAction(action);
    connect( oneOffgroup, SIGNAL(triggered(QAction*)), this, SLOT(newGloss(QAction*)) );
    submenu->addAction(action);


    QActionGroup *oneOffgroup2 = new QActionGroup(menu);
    action = new QAction(tr("Copy from baseline text..."),menu);
    action->setData( writingSystem.id() );
    oneOffgroup2->addAction(action);
    connect( oneOffgroup2, SIGNAL(triggered(QAction*)), this, SLOT(copyGlossFromBaseline(QAction*)) );
    submenu->addAction(action);

    menu->addMenu(submenu);
}

void WordDisplayWidget::addSearchSubmenu(QMenu *menu)
{
    QMenu *submenu = new QMenu(tr("Search for...") ,menu);
    QAction *action;
    QActionGroup *group;

    action = new QAction( tr("Interpretation %1").arg( mGlossItem->id() ) , menu );
    action->setData( mGlossItem->id() );
    group = new QActionGroup(menu);
    group->addAction(action);
    submenu->addAction(action);
    connect(group, SIGNAL(triggered(QAction*)), this, SLOT(interpretationSearch(QAction*)) );

    TextBitHashIterator iter( *mGlossItem->textForms() );
    while(iter.hasNext())
    {
        iter.next();
        action = new QAction( tr("%1 (Text Form %2)").arg( iter.value().text() ).arg( iter.value().id() ) , menu );
        action->setData( iter.value().id() );
        group = new QActionGroup(menu);
        group->addAction(action);
        submenu->addAction(action);
        connect(group, SIGNAL(triggered(QAction*)), this, SLOT(textFormSearch(QAction*)) );
    }

    iter = TextBitHashIterator( *mGlossItem->glosses() );
    while(iter.hasNext())
    {
        iter.next();
        action = new QAction( tr("%1 (Gloss %2)").arg( iter.value().text() ).arg( iter.value().id() ) , menu );
        action->setData( iter.value().id() );
        group = new QActionGroup(menu);
        group->addAction(action);
        submenu->addAction(action);
        connect(group, SIGNAL(triggered(QAction*)), this, SLOT(glossSearch(QAction*)) );
    }

    menu->addMenu(submenu);
}

void WordDisplayWidget::newInterpretation()
{
    qlonglong id = mDbAdapter->newInterpretation( mGlossItem->baselineText() );
    mGlossItem->setInterpretation(id, true); // true since there are no forms in the database, and so the fields need to be cleared
    fillData();
}

void WordDisplayWidget::duplicateInterpretation()
{
    TextBitHash textForms = *mGlossItem->textForms();
    TextBitHash glosses = *mGlossItem->glosses();

    qlonglong id = mDbAdapter->newInterpretation( mGlossItem->baselineText() );
    mGlossItem->setInterpretation(id, true); // true since there are no forms in the database, and so the fields need to be cleared

    TextBitHashIterator iter(textForms);
    while( iter.hasNext() )
    {
        iter.next();
        if( iter.key() != mGlossItem->baselineWritingSystem() )
        {
            qlonglong newId = mDbAdapter->newTextForm(id, iter.value() );
            mGlossItem->setTextForm( mDbAdapter->textFormFromId(newId) );
        }
    }

    iter = TextBitHashIterator(glosses);
    while( iter.hasNext() )
    {
        iter.next();
        qlonglong newId = mDbAdapter->newGloss( id, iter.value() );
        mGlossItem->setGloss( mDbAdapter->glossFromId(newId) );
    }

    fillData();
}

void WordDisplayWidget::copyGlossFromBaseline(QAction *action)
{
    qlonglong wsId = action->data().toLongLong();
    WritingSystem ws = mDbAdapter->writingSystem(wsId);
    copyGlossFromBaseline(ws);
}

void WordDisplayWidget::copyGlossFromBaseline()
{
    QMutableHashIterator<WritingSystem, LingEdit*> iter(mGlossEdits);
    while( iter.hasNext() )
    {
        iter.next();
        if( iter.value()->hasFocus() )
        {
            copyGlossFromBaseline( iter.key() );
            return;
        }
    }
}

void WordDisplayWidget::copyGlossFromBaseline(const WritingSystem & ws)
{
    TextBit bit = mGlossEdits[ws]->textBit();
    bit.setText( mGlossItem->textForm(mGlossItem->baselineWritingSystem()).text() );
    if( bit.id() == -1 )
    {
        qlonglong id = mDbAdapter->newGloss( mGlossItem->id() , bit );
        bit.setId( id );
    }

    mDbAdapter->updateGloss( bit );
    mGlossEdits[ws]->setTextBit( bit );
}


void WordDisplayWidget::newGloss(const WritingSystem & ws)
{
    GenericTextInputDialog dialog( ws , this );
    dialog.setWindowTitle(tr("New %1 gloss").arg(ws.name()));
    if( dialog.exec() == QDialog::Accepted )
    {
        TextBit newGloss = dialog.textBit();
        qlonglong id = mDbAdapter->newGloss( mGlossItem->id() , newGloss );
        newGloss.setId(id);
        mGlossItem->setGloss( newGloss );
        fillData();
    }
}

void WordDisplayWidget::newTextForm(const WritingSystem & ws)
{
    GenericTextInputDialog dialog( ws , this );
    dialog.setWindowTitle(tr("New %1 text form").arg(ws.name()));
    if( dialog.exec() == QDialog::Accepted )
    {
        TextBit newGloss = dialog.textBit();
        qlonglong id = mDbAdapter->newTextForm( mGlossItem->id() , newGloss );
        newGloss.setId(id);
        mGlossItem->setTextForm( newGloss );
        fillData();
    }
}


void WordDisplayWidget::newGloss(QAction *action)
{
    qlonglong wsId = action->data().toLongLong();
    WritingSystem ws = mDbAdapter->writingSystem(wsId);
    newGloss(ws);
}


void WordDisplayWidget::newTextForm(QAction *action)
{
    qlonglong wsId = action->data().toLongLong();
    WritingSystem ws = mDbAdapter->writingSystem(wsId);
    newTextForm(ws);
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
            case InterlinearItemType::ImmutableText:
                mImmutableLines[mGlossLines.at(i).writingSystem()]->setTextBit( mGlossItem->textForm( mGlossLines.at(i).writingSystem() ) );
                break;
            case InterlinearItemType::ImmutableGloss:
                mImmutableLines[mGlossLines.at(i).writingSystem()]->setTextBit( mGlossItem->gloss( mGlossLines.at(i).writingSystem() ) );
                break;
            case InterlinearItemType::Analysis:
                mAnalysisWidgets[mGlossLines.at(i).writingSystem()]->setupLayout();
                break;
            case InterlinearItemType::Null:
                break;
            }
        }
    }
}

void WordDisplayWidget::mouseDoubleClickEvent ( QMouseEvent * event )
{
    Q_UNUSED(event);
    mGlossItem->toggleApproval();
}

void WordDisplayWidget::mousePressEvent ( QMouseEvent * event )
{
    Q_UNUSED(event);
    emit leftClicked(this);
}

void WordDisplayWidget::selectDifferentCandidate(QAction *action)
{
    mGlossItem->setInterpretation( action->data().toLongLong() , true ); // true because the widget should start with at least some fields filled in
}

void WordDisplayWidget::selectDifferentGloss(QAction *action)
{
    TextBit bit = mDbAdapter->glossFromId( action->data().toLongLong() );
    mGlossItem->setGloss( bit );
}

void WordDisplayWidget::selectDifferentTextForm(QAction *action)
{
    TextBit bit = mDbAdapter->textFormFromId(action->data().toLongLong());
    mGlossItem->setTextForm( bit );
}

void WordDisplayWidget::textFormSearch(QAction * action)
{
    emit requestTextFormSearch( action->data().toLongLong() );
}

void WordDisplayWidget::glossSearch(QAction * action)
{
    emit requestGlossSearch( action->data().toLongLong() );
}

void WordDisplayWidget::interpretationSearch(QAction * action)
{
    emit requestInterpretationSearch( action->data().toLongLong() );
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

GlossItem* WordDisplayWidget::glossItem()
{
    return mGlossItem;
}

void WordDisplayWidget::displayDatabaseReport()
{
    QString reportString;

    reportString += tr("Interpretation ID: %1\n").arg( mGlossItem->id() );

    TextBitHashIterator iter( *mGlossItem->textForms() );
    while(iter.hasNext())
    {
        iter.next();
        reportString += tr("%1: %2\n").arg( iter.key().name() ).arg( iter.value().id() );
    }

    iter = TextBitHashIterator( *mGlossItem->glosses() );
    while(iter.hasNext())
    {
        iter.next();
        reportString += tr("%1: %2\n").arg( iter.key().name() ).arg( iter.value().id() );
    }

    QMessageBox::information(this, tr("Report"), reportString );
}

void WordDisplayWidget::editBaselineText()
{
    GenericTextInputDialog dialog( mGlossItem->baselineText() , this );
    dialog.setWindowTitle(tr("Edit baseline text (%1)").arg(mGlossItem->baselineText().writingSystem().name()));
    if( dialog.exec() == QDialog::Accepted )
    {
        QString text = dialog.text().trimmed();
        editBaselineText( text );
    }
}

void WordDisplayWidget::editBaselineText(const QString & text)
{
    if( text.contains(QRegExp("[ ]+")) )
    {
        DealWithSpacesDialog dealDialog(this);
        dealDialog.exec();
        if( dealDialog.choice() == DealWithSpacesDialog::SplitWord )
        {
            QList<TextBit> bits;
            QStringList portions = text.split(QRegExp("[ ]+"));
            for(int i=0; i<portions.count(); i++)
                bits << TextBit( portions.at(i), mGlossItem->baselineWritingSystem() );
            emit splitWidget( mGlossItem , bits  );
        }
        else if( dealDialog.choice() == DealWithSpacesDialog::ConvertSpaces )
        {
            QString replaced = text;
            replaced.replace(QChar(0x0020), QChar(0x00A0));
            TextBit bit( replaced , mGlossItem->baselineWritingSystem() );
            mGlossItem->resetBaselineText( bit );
        }
        else
        {
            return;
        }
    }
    else
    {
        TextBit bit( text , mGlossItem->baselineWritingSystem() );
        mGlossItem->resetBaselineText( bit );
    }
}

void WordDisplayWidget::editBaselineTextKeepAnnotations()
{
    GenericTextInputDialog dialog( mGlossItem->baselineText() , this );
    dialog.setWindowTitle(tr("Edit baseline text (%1)").arg(mGlossItem->baselineWritingSystem().name()));
    if( dialog.exec() == QDialog::Accepted )
    {
        QString text = dialog.text().trimmed();

        QList<qlonglong> candidates = mDbAdapter->candidateInterpretations( TextBit(text, mGlossItem->baselineWritingSystem() ) );
        if( candidates.count() > 0 )
        {
            if( QMessageBox::Yes == QMessageBox::question(this, tr("Use existing interpretation?"), tr("There is at least one interpretation for that text already. Would you like to use it instead of creating a new one?"), QMessageBox::Yes | QMessageBox::No , QMessageBox::Yes ) )
            {
                mGlossItem->setInterpretation( candidates.first(), true );
                return;
            }
        }

        TextBitHash textForms = *mGlossItem->textForms();
        TextBitHash glosses = *mGlossItem->glosses();

        editBaselineText( text );

        TextBitHashIterator iter(textForms);
        while( iter.hasNext() )
        {
            iter.next();
            if( iter.key() != mGlossItem->baselineWritingSystem() )
                mGlossItem->setTextForm( iter.value() );
        }

        iter = TextBitHashIterator(glosses);
        while( iter.hasNext() )
        {
            iter.next();
            mGlossItem->setGloss( iter.value() );
        }
    }
}

void WordDisplayWidget::splitIntoMultipleWords()
{
    GenericTextInputDialog dialog( mGlossItem->baselineText() , this );
    dialog.setWindowTitle(tr("Split the word with spaces"));
    if( dialog.exec() == QDialog::Accepted )
    {
        QString text = dialog.text().trimmed();
        int whitespaceCount = text.count(QRegExp("\\s+"));
        if( whitespaceCount > 0 )
        {
            QList<TextBit> bits;
            QStringList portions = text.split(QRegExp("\\s+"));
            for(int i=0; i<portions.count(); i++)
                bits << TextBit( portions.at(i), mGlossItem->baselineWritingSystem() );
            emit splitWidget( mGlossItem , bits  );
        }
    }
}

void WordDisplayWidget::mergeWithNext()
{
    emit mergeGlossItemWithNext( mGlossItem );
}

void WordDisplayWidget::mergeWithPrevious()
{
    emit mergeGlossItemWithPrevious( mGlossItem );
}

void WordDisplayWidget::removeGlossItem()
{
    if( QMessageBox::Yes == QMessageBox::question(this, tr("Confirm deletion"), tr("Are you sure you want to remove this gloss item? This cannot be undone."), QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) )
        emit requestRemoveGlossItem(mGlossItem);
}

void WordDisplayWidget::setFocused(bool isFocused)
{
    if( isFocused )
        setStyleSheet("WordDisplayWidget { border: 1px solid red; }");
    else
        setStyleSheet("WordDisplayWidget { border: none; }");
}

void WordDisplayWidget::receiveKeyboardFocus()
{
    for(int i=0; i<mGlossLines.count(); i++)
    {
        if( mGlossLines.at(i).type() == InterlinearItemType::Text)
        {
            mTextFormEdits.value( mGlossLines.at(i).writingSystem() )->setFocus();
            return;
        }
        else if (mGlossLines.at(i).type() ==  InterlinearItemType::Gloss )
        {
            mGlossEdits.value( mGlossLines.at(i).writingSystem() )->setFocus();
            return;
        }
    }
}

void WordDisplayWidget::setSelected(bool selected)
{
    if( selected )
    {
        setStyleSheet("WordDisplayWidget { border: 1px solid #c0c0c0; background-color: transparent; }");
    }
    else
    {
        setStyleSheet("WordDisplayWidget { border: 1px solid transparent; background-color: transparent; }");
    }
}

void WordDisplayWidget::keyPressEvent ( QKeyEvent * event )
{
    int key = event->key();
    if( key >= Qt::Key_F1 && key <= Qt::Key_F35 )
    {
        key -= Qt::Key_F1; // now F1 is zero, F2 is one, etc.

        if( key < mGlossLines.count() )
        {
            if( key == 0 ) // F1 is reserved for toggling interpretations
            {
                cycleInterpretation();
            }
            else if( mGlossLines.at(key).type() == InterlinearItemType::Text )
            {
                cycleTextForm( mGlossLines.at(key).writingSystem() );
            }
            else if( mGlossLines.at(key).type() == InterlinearItemType::Gloss )
            {
                cycleGloss( mGlossLines.at(key).writingSystem() );
            }
        }
    }
    else
      {
        QWidget::keyPressEvent(event);
      }
}

void WordDisplayWidget::cycleInterpretation()
{
    QList<qlonglong> candidates = mDbAdapter->candidateInterpretations( mGlossItem->baselineText() );
    qSort(candidates.begin(), candidates.end());
    int position = candidates.indexOf( mGlossItem->id() );
    if( position > -1 )
    {
        position++;
        if( position == candidates.count() )
            position = 0;
        mGlossItem->setInterpretation( candidates.at(position) , true );
    }
}

void WordDisplayWidget::cycleTextForm( const WritingSystem & ws )
{
    QHash<qlonglong,QString> textForms = mDbAdapter->interpretationTextForms(mGlossItem->id(), ws.id() );
    QList<qlonglong> candidates = textForms.uniqueKeys();
    qSort( candidates.begin(), candidates.end() );
    int position = candidates.indexOf( mGlossItem->textForm( ws ).id() );
    if( position > -1 )
    {
        position++;
        if( position == candidates.count() )
            position = 0;
        mGlossItem->setTextForm( mDbAdapter->textFormFromId( candidates.at(position) ) );
    }
}

void WordDisplayWidget::cycleGloss( const WritingSystem & ws )
{
    QHash<qlonglong,QString> glosses = mDbAdapter->interpretationGlosses( mGlossItem->id(), ws.id() );
    QList<qlonglong> candidates = glosses.uniqueKeys();
    qSort( candidates.begin(), candidates.end() );
    int position = candidates.indexOf( mGlossItem->gloss( ws ).id() );
    if( position > -1 )
    {
        position++;
        if( position == candidates.count() )
            position = 0;
        mGlossItem->setGloss( mDbAdapter->glossFromId( candidates.at(position) ) );
    }
}

void WordDisplayWidget::guessGloss()
{
    QHashIterator<WritingSystem, LingEdit*> iter(mGlossEdits);
    while(iter.hasNext())
    {
        iter.next();
        if( iter.value()->hasFocus() )
        {
            guessGloss( iter.key() );
            return;
        }
    }
}

void WordDisplayWidget::guessGloss( const WritingSystem & ws )
{
    QString hint;
    QHashIterator<WritingSystem, LingEdit*> iter(mGlossEdits);
    while(iter.hasNext())
    {
        iter.next();
        if( iter.key() != ws && !iter.value()->text().isEmpty() )
        {
            hint = iter.value()->text();
            break;
        }
    }
    if( !hint.isEmpty() )
    {
        QString guess = mDbAdapter->guessGloss( hint , ws );
        if( !guess.isEmpty() )
        {
            mGlossEdits.value(ws)->setText( guess );
        }
    }
}

void WordDisplayWidget::approveLine()
{
    emit requestApproveLine(this);
}

void WordDisplayWidget::rightGlossItem()
{
    emit requestLeftGlossItem(this);
}

void WordDisplayWidget::leftGlossItem()
{
    emit requestRightGlossItem(this);
}

void WordDisplayWidget::playSound()
{
    emit requestPlaySound(this);
}

void WordDisplayWidget::matchFollowingTextFormsToThis()
{
    emit requestSetFollowingInterpretations( mGlossItem );
}

void WordDisplayWidget::editBaselineTextMatchingFollowing()
{
    QString oldText = mGlossItem->baselineText().text();
    editBaselineText();
    emit requestReplaceFollowing( mGlossItem, oldText );
}

void WordDisplayWidget::editBaselineTextForm()
{
    GenericTextInputDialog dialog( mGlossItem->baselineText() , this );
    dialog.setWindowTitle(tr("Edit Text Form %1").arg( mGlossItem->baselineText().id() ));
    if( dialog.exec() == QDialog::Accepted )
    {
        mDbAdapter->updateTextForm( dialog.textBit() );
        mGlossItem->setTextFormText( dialog.textBit() );
    }
}

void WordDisplayWidget::annotationMarkActivated( const QString & key )
{
    TextBit annotation = mGlossItem->getAnnotation( key );
    if( annotation.text().isEmpty() )
        annotation.setWritingSystem( mProject->annotationType(key).writingSystem() );
    if( annotation.writingSystem().isNull() )
        return;

    GenericTextInputDialog dialog(annotation, this);
    dialog.setWindowTitle(key);
    if( dialog.exec() == QDialog::Accepted )
    {
        mGlossItem->setAnnotation( key , dialog.textBit() );
        mAnnotationMarks->setupLayout();
    }
}

void WordDisplayWidget::changeFollowingToMatchTextForm(QAction *action)
{
    emit requestSetFollowingTextForms( mGlossItem , mDbAdapter->writingSystem( action->data().toLongLong() ) );
}

void WordDisplayWidget::changeFollowingToMatchGloss(QAction *action)
{
    emit requestSetFollowingGlosses( mGlossItem , mDbAdapter->writingSystem( action->data().toLongLong() ) );
}
