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
#include "annotationeditordialog.h"

#include <QtWidgets>
#include <QtDebug>
#include <QActionGroup>

WordDisplayWidget::WordDisplayWidget(GlossItem *item, Qt::Alignment alignment, const Tab * tab, Project * project, QWidget *parent) :
    QFrame(parent),
    mProject(project),
    mDbAdapter(project->dbAdapter()),
    mGlossItem(item),
    mConcordance(mGlossItem->concordance()),
    mAlignment(alignment),
    mGlossLines(tab->interlinearLines(mGlossItem->baselineWritingSystem()))
{
    setStyleSheet("WordDisplayWidget { border: 1px solid transparent; background-color: transparent; }");

    setObjectName("WordDisplayWidget");

    setupLayout();

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));

    connect( mGlossItem, SIGNAL(fieldsChanged()), this, SLOT(fillData()), Qt::UniqueConnection );
    connect( this, SIGNAL(requestInterpretationSearch(qlonglong)), mGlossItem->project()->mainWindow(), SLOT( searchForInterpretationById(qlonglong) ));
    connect( this, SIGNAL(requestTextFormSearch(qlonglong)), mGlossItem->project()->mainWindow(), SLOT( searchForTextFormById(qlonglong) ));
    connect( this, SIGNAL(requestGlossSearch(qlonglong)), mGlossItem->project()->mainWindow(), SLOT( searchForGlossById(qlonglong) ));
    connect( this, SIGNAL(requestAllomorphSearch(qlonglong)), mGlossItem->project()->mainWindow(), SLOT( searchForAllomorphById(qlonglong)) );
    connect( this, SIGNAL(requestLexicalEntrySearch(qlonglong)), mGlossItem->project()->mainWindow(), SLOT( searchForLexicalEntryById(qlonglong)) );

    connect( mGlossItem, SIGNAL(glossNumberChanged(bool,qlonglong,WritingSystem)), this, SLOT(setGlossNumber(bool,qlonglong,WritingSystem)) );
    connect( mGlossItem, SIGNAL(textFormNumberChanged(bool,qlonglong,WritingSystem)), this, SLOT(setTextFormNumber(bool,qlonglong,WritingSystem)) );

    setupShortcuts();

    fillData();
}

WordDisplayWidget::~WordDisplayWidget()
{
}

void WordDisplayWidget::setupLayout()
{
    if( mGlossLines->isEmpty() ) return;

    QHBoxLayout *hLayout = new QHBoxLayout;

    mAnnotationMarks = new AnnotationMarkWidget( mProject->annotationTypes() , mGlossItem );
    connect( mAnnotationMarks, SIGNAL(annotationActivated(QString)), this, SLOT(annotationMarkActivated(QString)) );

    QVBoxLayout *vLayout = new QVBoxLayout;

    if( mGlossLines->first().writingSystem().layoutDirection() == Qt::LeftToRight )
    {
        hLayout->addLayout(vLayout);
        hLayout->addWidget(mAnnotationMarks);
    }
    else
    {
        hLayout->addWidget(mAnnotationMarks);
        hLayout->addLayout(vLayout);
    }

    setLayout(hLayout);

    mTextFormEdits.clear();
    mGlossEdits.clear();
    mImmutableLines.clear();
    mAnalysisWidgets.clear();

    for(int i=0; i<mGlossLines->count(); i++)
    {
        switch( mGlossLines->at(i).type() )
        {
        case InterlinearItemType::ImmutableText:
            vLayout->addWidget( addImmutableTextFormLine( mGlossLines->at(i) , i == 0 ) );
            break;
        case InterlinearItemType::ImmutableGloss:
            vLayout->addWidget( addImmutableGlossLine( mGlossLines->at(i) , i == 0 ) );
            break;
        case InterlinearItemType::Text:
            vLayout->addWidget( addTextFormLine( mGlossLines->at(i) ) );
            break;
        case InterlinearItemType::Gloss:
            vLayout->addWidget( addGlossLine( mGlossLines->at(i) ) );
            break;
        case InterlinearItemType::Analysis:
            vLayout->addWidget( addAnalysisWidget( mGlossLines->at(i) ) );
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

    QAction *approvePhrase = new QAction(this);
    approvePhrase->setShortcut( QKeySequence("Ctrl+Shift+A") );
    approvePhrase->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect( approvePhrase, SIGNAL(triggered()), this, SLOT(approvePhrase()) );
    addAction(approvePhrase);

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
    TextBit gloss("", glossLine.writingSystem());
    LingEdit *edit = new LingEdit( gloss , this);
    edit->matchTextAlignmentTo( mGlossLines->first().writingSystem().layoutDirection() );

    mGlossEdits.insert(glossLine.writingSystem(), edit);

    // update the gloss item
    connect(edit,SIGNAL(stringChanged(TextBit,LingEdit*)), mGlossItem, SLOT(setGloss(TextBit)) );
    connect(edit, SIGNAL(insertPressed(WritingSystem)), this, SLOT(newGloss(WritingSystem)));
    connect(edit, SIGNAL(stringChanged(TextBit,LingEdit*)), this, SLOT(checkForDuplicateGloss(TextBit,LingEdit*)) );

    return edit;
}

LingEdit* WordDisplayWidget::addTextFormLine( const InterlinearItemType & glossLine )
{
    TextBit textForm("", glossLine.writingSystem() );
    LingEdit *edit = new LingEdit(  textForm , this);
    edit->matchTextAlignmentTo( mGlossLines->first().writingSystem().layoutDirection() );

    mTextFormEdits.insert(glossLine.writingSystem(), edit);

    // update the gloss item
    connect(edit,SIGNAL(stringChanged(TextBit,LingEdit*)), mGlossItem, SLOT(setTextForm(TextBit)) );
    connect(edit, SIGNAL(insertPressed(WritingSystem)), this, SLOT(newTextForm(WritingSystem)));
    connect(edit, SIGNAL(stringChanged(TextBit,LingEdit*)), this, SLOT(checkForDuplicateTextForm(TextBit,LingEdit*)) );

    return edit;
}

ImmutableLabel* WordDisplayWidget::addImmutableTextFormLine( const InterlinearItemType & glossLine, bool technicolor )
{
    TextBit bit("", glossLine.writingSystem());

    ImmutableLabel *immutableLabel = new ImmutableLabel( bit, technicolor , this);
    immutableLabel->setCandidateNumberAndApprovalStatus(mGlossItem->candidateNumber(), mGlossItem->approvalStatus());
    immutableLabel->matchTextAlignmentTo( mGlossLines->first().writingSystem().layoutDirection() );

    mImmutableLines.insert( glossLine.writingSystem() , immutableLabel );

    connect( mGlossItem, SIGNAL(approvalStatusChanged(GlossItem::ApprovalStatus)), immutableLabel, SLOT(setApprovalStatus(GlossItem::ApprovalStatus)) );
    connect( mGlossItem, SIGNAL(candidateNumberChanged(GlossItem::CandidateNumber,qlonglong)), immutableLabel, SLOT(setCandidateNumber(GlossItem::CandidateNumber)) );

    if( technicolor )
    {
        connect( immutableLabel, SIGNAL(doubleClick(TextBit&)), mGlossItem, SLOT(toggleApproval()) );
    }

    return immutableLabel;
}

ImmutableLabel* WordDisplayWidget::addImmutableGlossLine( const InterlinearItemType & glossLine, bool technicolor )
{
    TextBit bit("", glossLine.writingSystem());

    ImmutableLabel *immutableLabel = new ImmutableLabel( bit, technicolor , this);
    immutableLabel->setCandidateNumberAndApprovalStatus(mGlossItem->candidateNumber(), mGlossItem->approvalStatus());
    immutableLabel->matchTextAlignmentTo( mGlossLines->first().writingSystem().layoutDirection() );

    mImmutableLines.insert( glossLine.writingSystem() , immutableLabel );

    connect( mGlossItem, SIGNAL(approvalStatusChanged(GlossItem::ApprovalStatus)), immutableLabel, SLOT(setApprovalStatus(GlossItem::ApprovalStatus)) );
    connect( mGlossItem, SIGNAL(candidateNumberChanged(GlossItem::CandidateNumber,qlonglong)), immutableLabel, SLOT(setCandidateNumber(GlossItem::CandidateNumber)) );

    if( technicolor )
    {
        connect( immutableLabel, SIGNAL(doubleClick(TextBit&)), mGlossItem, SLOT(toggleApproval()) );
    }

    return immutableLabel;
}

AnalysisWidget* WordDisplayWidget::addAnalysisWidget( const InterlinearItemType & glossLine )
{
    AnalysisWidget *analysisWidget = new AnalysisWidget(mGlossItem, glossLine.writingSystem(), mGlossLines->first().writingSystem().layoutDirection(), mProject, this);
    mAnalysisWidgets.insert( glossLine.writingSystem(), analysisWidget );

    connect( mGlossItem, SIGNAL(morphologicalAnalysisChanged(const GlossItem*,const MorphologicalAnalysis*)), analysisWidget, SLOT(setupLayout()));
    connect( analysisWidget, SIGNAL(morphologicalAnalysisChanged(const GlossItem*,const MorphologicalAnalysis*)), mConcordance, SLOT(updateGlossItemMorphologicalAnalysis(const GlossItem*,const MorphologicalAnalysis*)) );
    connect( analysisWidget, SIGNAL(requestAlternateInterpretation()), this, SLOT(duplicateInterpretation()) );

    return analysisWidget;
}

void WordDisplayWidget::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu *menu = new QMenu(this);

    menu->addAction(tr("Copy baseline text"), this, SLOT(copyBaselineText()));
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

    menu->addAction(tr("Begin new line here"), this, SLOT(beginNewPhraseHere()));
    menu->addAction(tr("Merge this line with previous"), this, SLOT(noNewPhraseHere()));

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


    for(int i=0; i<mGlossLines->count(); i++)
    {
        switch( mGlossLines->at(i).type() )
        {
        case InterlinearItemType::Text:
        case InterlinearItemType::ImmutableText:
            addTextFormSubmenu( menu , mGlossLines->at(i).writingSystem() );
            break;
        case InterlinearItemType::Gloss:
        case InterlinearItemType::ImmutableGloss:
            addGlossSubmenu( menu , mGlossLines->at(i).writingSystem() );
            break;
        case InterlinearItemType::Analysis:
        case InterlinearItemType::Null:
            break;
        }
    }

    menu->addSeparator();

    addAllomorphEditSubmenu(menu);
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

    action = new QAction( tr("Interpretation %1 (%2x)").arg( mGlossItem->id() ).arg(mDbAdapter->interpretationCountFromConcordance(mGlossItem->id())) , menu );
    action->setData( mGlossItem->id() );
    group = new QActionGroup(menu);
    group->addAction(action);
    submenu->addAction(action);
    connect(group, SIGNAL(triggered(QAction*)), this, SLOT(interpretationSearch(QAction*)) );

    TextBitHashIterator iter( *mGlossItem->textForms() );
    while(iter.hasNext())
    {
        iter.next();
        action = new QAction( tr("%1 (Text Form %2; %3x)").arg( iter.value().text() ).arg( iter.value().id() ).arg( mDbAdapter->textFormCountFromConcordance( iter.value().id() ) ) , menu );
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
        action = new QAction( tr("%1 (Gloss %2; %3x)").arg( iter.value().text() ).arg( iter.value().id() ).arg( mDbAdapter->glossCountFromConcordance( iter.value().id() ) ) , menu );
        action->setData( iter.value().id() );
        group = new QActionGroup(menu);
        group->addAction(action);
        submenu->addAction(action);
        connect(group, SIGNAL(triggered(QAction*)), this, SLOT(glossSearch(QAction*)) );
    }

    QListIterator<WritingSystem> maWS(mGlossItem->nonEmptyMorphologicalAnalysisLanguages());

    if( maWS.hasNext() )
    {
        submenu->addSeparator();
    }

    while( maWS.hasNext() )
    {
        AllomorphPointerIterator ai = mGlossItem->morphologicalAnalysis( maWS.next() )->allomorphIterator();
        while( ai.hasNext() )
        {
            const Allomorph * a = ai.next();
            action = new QAction( tr("%1 (Lexical Entry %2)").arg( a->text() ).arg( a->lexicalEntryId() ) , menu );
            action->setData( a->lexicalEntryId() );
            group = new QActionGroup(menu);
            group->addAction(action);
            submenu->addAction(action);
            connect(group, SIGNAL(triggered(QAction*)), this, SLOT(lexicalEntrySearch(QAction*)) );
        }
        ai.toFront();
        submenu->addSeparator();

        while( ai.hasNext() )
        {
            const Allomorph * a = ai.next();
            action = new QAction( tr("%1 (Allomorph %2)").arg( a->text() ).arg( a->id() ) , menu );
            action->setData( a->id() );
            group = new QActionGroup(menu);
            group->addAction(action);
            submenu->addAction(action);
            connect(group, SIGNAL(triggered(QAction*)), this, SLOT(allomorphSearch(QAction*)) );
        }

        if( maWS.hasNext() )
        {
            submenu->addSeparator();
        }
    }

    menu->addMenu(submenu);
}

void WordDisplayWidget::addAllomorphEditSubmenu(QMenu *menu)
{
    QMenu *submenu = new QMenu(tr("Edit allomorphs...") ,menu);
    QAction *action;
    QActionGroup *group;

    QListIterator<WritingSystem> maWS(mGlossItem->nonEmptyMorphologicalAnalysisLanguages());

    if( maWS.hasNext() )
    {
        submenu->addSeparator();
    }

    while( maWS.hasNext() )
    {
        AllomorphPointerIterator ai = mGlossItem->morphologicalAnalysis( maWS.next() )->allomorphIterator();

        while( ai.hasNext() )
        {
            const Allomorph * a = ai.next();
            action = new QAction( tr("%1 (Allomorph %2)").arg( a->text() ).arg( a->id() ) , menu );
            action->setData( a->id() );
            group = new QActionGroup(menu);
            group->addAction(action);
            submenu->addAction(action);
            connect(group, SIGNAL(triggered(QAction*)), this, SLOT(allomorphEdit(QAction*)) );
        }

        if( maWS.hasNext() )
        {
            submenu->addSeparator();
        }
    }

    menu->addMenu(submenu);
}

void WordDisplayWidget::newInterpretation()
{
    qlonglong id = mDbAdapter->newInterpretation( mGlossItem->baselineText() );
    mGlossItem->setInterpretation(id);
    fillData();
}

void WordDisplayWidget::duplicateInterpretation()
{
    TextBitHash textForms = *mGlossItem->textForms();
    TextBitHash glosses = *mGlossItem->glosses();

    qlonglong id = mDbAdapter->newInterpretation( mGlossItem->baselineText() );
    mGlossItem->setInterpretation(id);

    TextBitHashIterator iter(textForms);
    while( iter.hasNext() )
    {
        iter.next();
        if( iter.key() != mGlossItem->baselineWritingSystem() )
        {
            mGlossItem->setTextForm( mDbAdapter->newTextForm(id, iter.value() ) );
        }
    }

    iter = TextBitHashIterator(glosses);
    while( iter.hasNext() )
    {
        iter.next();
        mGlossItem->setGloss( mDbAdapter->newGloss( id, iter.value() ) );
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
        qlonglong id = mDbAdapter->newGloss( mGlossItem->id() , bit ).id();
        bit.setId( id );
    }

    mDbAdapter->updateGloss( bit );
    mGlossEdits[ws]->setTextBit( bit );
}


void WordDisplayWidget::newGloss(const WritingSystem & ws)
{
    GenericTextInputDialog dialog( ws , this );
    dialog.setWindowTitle(tr("New %1 gloss").arg(ws.name()));
    dialog.suggestInput( mGlossEdits.value(ws)->textBit() );
    if( dialog.exec() == QDialog::Accepted )
    {
        mGlossItem->setGloss( mDbAdapter->newGloss( mGlossItem->id() , dialog.textBit() ) );
        mGlossEdits.value(ws)->setSpecialBorder(true);
        fillData();
    }
}

void WordDisplayWidget::newTextForm(const WritingSystem & ws)
{
    GenericTextInputDialog dialog( ws , this );
    dialog.setWindowTitle(tr("New %1 text form").arg(ws.name()));
    dialog.suggestInput( mTextFormEdits.value(ws)->textBit() );
    if( dialog.exec() == QDialog::Accepted )
    {
        mGlossItem->setTextForm( mDbAdapter->newTextForm( mGlossItem->id() , dialog.textBit() ) );
        mTextFormEdits.value(ws)->setSpecialBorder(true);
        fillData();
    }
}

void WordDisplayWidget::setTextFormNumber(bool multipleAvailable, qlonglong interpretationId, const WritingSystem &ws)
{
    Q_UNUSED(interpretationId);
    if( mTextFormEdits.contains(ws) )
    {
        mTextFormEdits[ws]->setSpecialBorder(multipleAvailable);
    }
}

void WordDisplayWidget::setGlossNumber(bool multipleAvailable, qlonglong interpretationId, const WritingSystem &ws)
{
    Q_UNUSED(interpretationId);
    if( mGlossEdits.contains(ws) )
    {
        mGlossEdits[ws]->setSpecialBorder(multipleAvailable);
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
        for(int i=0; i<mGlossLines->count();i++)
        {
            switch( mGlossLines->at(i).type() )
            {
            case InterlinearItemType::Text:
                mTextFormEdits[mGlossLines->at(i).writingSystem()]->setTextBit( mGlossItem->textForm(mGlossLines->at(i).writingSystem()) );
                mTextFormEdits[mGlossLines->at(i).writingSystem()]->setSpecialBorder( mGlossItem->multipleTextFormsAvailable( mGlossLines->at(i).writingSystem() ) );
                break;
            case InterlinearItemType::Gloss:
                mGlossEdits[mGlossLines->at(i).writingSystem()]->setTextBit( mGlossItem->gloss( mGlossLines->at(i).writingSystem() ) );
                mGlossEdits[mGlossLines->at(i).writingSystem()]->setSpecialBorder( mGlossItem->multipleGlossesAvailable( mGlossLines->at(i).writingSystem() ) );
                break;
            case InterlinearItemType::ImmutableText:
                mImmutableLines[mGlossLines->at(i).writingSystem()]->setTextBit( mGlossItem->textForm( mGlossLines->at(i).writingSystem() ) );
                break;
            case InterlinearItemType::ImmutableGloss:
                mImmutableLines[mGlossLines->at(i).writingSystem()]->setTextBit( mGlossItem->gloss( mGlossLines->at(i).writingSystem() ) );
                break;
            case InterlinearItemType::Analysis:
                mAnalysisWidgets[mGlossLines->at(i).writingSystem()]->setupLayout();
                break;
            case InterlinearItemType::Null:
                break;
            }
        }
    }
}

void WordDisplayWidget::mousePressEvent ( QMouseEvent * event )
{
    Q_UNUSED(event);
    emit leftClicked(this);
}

void WordDisplayWidget::selectDifferentCandidate(QAction *action)
{
    mGlossItem->setInterpretation( action->data().toLongLong() );
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

void WordDisplayWidget::lexicalEntrySearch(QAction *action)
{
    emit requestLexicalEntrySearch( action->data().toLongLong() );
}

void WordDisplayWidget::allomorphSearch(QAction *action)
{
    emit requestAllomorphSearch( action->data().toLongLong() );
}

void WordDisplayWidget::allomorphEdit(QAction *action)
{
    qlonglong allomorphId = action->data().toLongLong();
    Allomorph * a = mDbAdapter->allomorphFromId(allomorphId);
    GenericTextInputDialog dialog( a->textBit() , this );
    dialog.setWindowTitle(tr("Edit allomorph (%1)").arg(a->id()));
    if( dialog.exec() == QDialog::Accepted )
    {
        QStringList changes = mDbAdapter->proposeEditTextFormsFromAllomorph(allomorphId,a->textBit(),dialog.textBit());
        if( !changes.isEmpty() ) // this should always pass
        {
            if( QMessageBox::Yes == QMessageBox::warning(this, tr("Really?"), tr("<p>This will perform a search-and-replace on the text forms that have this allomorph. This could produce a lot of random effects, depending on the allomorph you're editing. Review the list of changes below to see if you want to do this. <ul><li>%1</li></ul><p>If you go ahead with this, note that the text in the morphological analysis display will not be updated until you close and re-open the text.</p>").arg(changes.join("</li><li>")) , QMessageBox::Yes | QMessageBox::No , QMessageBox::No ) )
            {
                QList<TextBit> affected = mDbAdapter->editTextFormsFromAllomorph(allomorphId,a->textBit(),dialog.textBit());
                QListIterator<TextBit> i(affected);
                while(i.hasNext())
                {
                    // make sure that these changes are propagated throughout all gloss items
                    mGlossItem->emitTextFormChangedSignal( i.next() );
                }
            }
        }
    }
    delete a;
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

void WordDisplayWidget::copyBaselineText()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText( mGlossItem->baselineText().text() );
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
                mGlossItem->setInterpretation( candidates.first() );
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
    for(int i=0; i<mGlossLines->count(); i++)
    {
        if( mGlossLines->at(i).type() == InterlinearItemType::Text)
        {
            mTextFormEdits.value( mGlossLines->at(i).writingSystem() )->setFocus();
            return;
        }
        else if (mGlossLines->at(i).type() ==  InterlinearItemType::Gloss )
        {
            mGlossEdits.value( mGlossLines->at(i).writingSystem() )->setFocus();
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

        if( key < mGlossLines->count() )
        {
            if( key == 0 ) // F1 is reserved for toggling interpretations
            {
                cycleInterpretation();
            }
            else if( mGlossLines->at(key).type() == InterlinearItemType::Text )
            {
                cycleTextForm( mGlossLines->at(key).writingSystem() );
            }
            else if( mGlossLines->at(key).type() == InterlinearItemType::Gloss )
            {
                cycleGloss( mGlossLines->at(key).writingSystem() );
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
    /// save all current textforms and glosses since we don't know what's changed
    QHashIterator<WritingSystem, LingEdit*> iter(mTextFormEdits);
    while(iter.hasNext())
    {
        iter.next();
        mGlossItem->setTextForm( iter.value()->textBit() );
    }

    QHashIterator<WritingSystem, LingEdit*> iter2(mGlossEdits);
    while(iter2.hasNext())
    {
        iter2.next();
        mGlossItem->setGloss( iter2.value()->textBit() );
    }

    QList<qlonglong> candidates = mDbAdapter->candidateInterpretations( mGlossItem->baselineText() );
    qSort(candidates.begin(), candidates.end());
    int position = candidates.indexOf( mGlossItem->id() );
    if( position > -1 )
    {
        position++;
        if( position == candidates.count() )
            position = 0;
        mGlossItem->setInterpretation( candidates.at(position) );
    }
}

void WordDisplayWidget::cycleTextForm( const WritingSystem & ws )
{
    /// save the current string
    mGlossItem->setTextForm( mTextFormEdits.value( ws )->textBit() );

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
    /// save the current string
    mGlossItem->setGloss( mGlossEdits.value( ws )->textBit() );

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

void WordDisplayWidget::approvePhrase()
{
    emit requestApprovePhrase(this);
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

void WordDisplayWidget::beginNewPhraseHere()
{
    emit requestNewPhraseFromHere( mGlossItem );
}

void WordDisplayWidget::noNewPhraseHere()
{
    emit requestNoPhraseFromHere( mGlossItem );
}

void WordDisplayWidget::checkForDuplicateTextForm(const TextBit &bit, LingEdit *edit)
{
    if( mDbAdapter->hasDuplicateTextForms(mGlossItem->id(), bit.writingSystem().id()) )
    {
        edit->setWarning(tr("Multiple text forms have the same string. This is likely a mistake."));
    }
    else
    {
        edit->setWarning();
    }
}

void WordDisplayWidget::checkForDuplicateGloss(const TextBit &bit, LingEdit *edit)
{
    if( mDbAdapter->hasDuplicateGlosses(mGlossItem->id(), bit.writingSystem().id()) )
    {
        edit->setWarning(tr("Multiple glosses have the same string. This is likely a mistake."));
    }
    else
    {
        edit->setWarning();
    }
}

void WordDisplayWidget::annotationMarkActivated( const QString & key )
{
    Annotation annotation = mGlossItem->getAnnotation( key );
    annotation.setWritingSystem( mProject->annotationType(key).writingSystem() );
    annotation.setHeaderWritingSystem( mProject->annotationType(key).headerWritingSystem() );

    AnnotationEditorDialog dialog( annotation, mGlossItem->baselineText(), this );
    dialog.setWindowTitle(key);
    if( dialog.exec() == QDialog::Accepted )
    {
        mGlossItem->setAnnotation( key , dialog.annotation() );
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
