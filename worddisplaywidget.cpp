#include "worddisplaywidget.h"
#include "textbit.h"
#include "project.h"
#include "lingedit.h"
#include "interlinearitemtype.h"
#include "databaseadapter.h"
#include "interlineardisplaywidget.h"
#include "interpretationsearchdialog.h"
#include "immutablelabel.h"
#include "analysiswidget.h"
#include "generictextinputdialog.h"
#include "mainwindow.h"

#include <QtGui>
#include <QtDebug>
#include <QActionGroup>

WordDisplayWidget::WordDisplayWidget( GlossItem *item, Qt::Alignment alignment, const QList<InterlinearItemType> & lines, DatabaseAdapter *dbAdapter, QWidget *parent) : QWidget(parent)
{
    mDbAdapter = dbAdapter;
    mGlossItem = item;
    mConcordance = mGlossItem->concordance();
    mAlignment = alignment;

    mGlossLines = lines;

    setMaximumWidth(100);

    setupLayout();

    setMinimumSize(128,20);

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));

    // Why would these be here?
    // because I can't put them in the GlossItem constructor, so this is the next best thing (after some kind of secondary initialization method)
    // http://www.qtcentre.org/threads/9479-connect-in-constructor
    // http://www.parashift.com/c++-faq-lite/link-errs-static-data-mems.html
    connect( mGlossItem, SIGNAL(fieldsChanged()), this, SLOT(fillData()), Qt::UniqueConnection );
    connect( mGlossItem, SIGNAL(destroyed(QObject*)), mConcordance, SLOT(removeGlossItemFromConcordance(QObject*)), Qt::UniqueConnection);
    connect( mGlossItem, SIGNAL(candidateNumberChanged(GlossItem::CandidateNumber,qlonglong)), mConcordance, SLOT(updateInterpretationsAvailableForGlossItem(GlossItem::CandidateNumber,qlonglong)), Qt::UniqueConnection);
    connect( mGlossItem, SIGNAL(textFormChanged(TextBit)), mConcordance, SLOT(updateTextForm(TextBit)), Qt::UniqueConnection);
    connect( mGlossItem, SIGNAL(glossChanged(TextBit)), mConcordance, SLOT(updateGloss(TextBit)), Qt::UniqueConnection);
    connect( mGlossItem, SIGNAL(morphologicalAnalysisChanged(MorphologicalAnalysis)), mConcordance, SLOT(updateGlossItemMorphologicalAnalysis(MorphologicalAnalysis)), Qt::UniqueConnection);

    connect( this, SIGNAL(requestInterpretationSearch(qlonglong)), mGlossItem->project()->mainWindow(), SLOT( searchForInterpretationById(qlonglong) ));
    connect( this, SIGNAL(requestTextFormSearch(qlonglong)), mGlossItem->project()->mainWindow(), SLOT( searchForTextFormById(qlonglong) ));
    connect( this, SIGNAL(requestGlossSearch(qlonglong)), mGlossItem->project()->mainWindow(), SLOT( searchForGlossById(qlonglong) ));

    fillData();
}

WordDisplayWidget::~WordDisplayWidget()
{
}

void WordDisplayWidget::setupLayout()
{
    mLayout = new QVBoxLayout(this);
    setLayout(mLayout);

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
            mLayout->addWidget(immutableLabel);
            break;
        case InterlinearItemType::ImmutableGloss:
            immutableLabel = addImmutableGlossLine( mGlossLines.at(i) , i == 0 );
            mLayout->addWidget(immutableLabel);
            break;
        case InterlinearItemType::Text:
            lineWidget = addTextFormLine( mGlossLines.at(i) );
            mLayout->addWidget(lineWidget);
            break;
        case InterlinearItemType::Gloss:
            lineWidget = addGlossLine( mGlossLines.at(i) );
            mLayout->addWidget(lineWidget);
            break;
        case InterlinearItemType::Analysis:
            analysisWidget = addAnalysisWidget( mGlossLines.at(i) );
            mLayout->addWidget(analysisWidget);
            break;
        case InterlinearItemType::Null:
            break;
        }
    }
}

LingEdit* WordDisplayWidget::addGlossLine( const InterlinearItemType & glossLine )
{
    TextBit gloss = mGlossItem->gloss( glossLine.writingSystem() );
    LingEdit *edit = new LingEdit( gloss , this);
    edit->matchTextAlignmentTo( mGlossLines.first().writingSystem().layoutDirection() );

    mGlossEdits.insert(glossLine.writingSystem(), edit);

    // update the concordance
    mConcordance->updateGlossLingEditConcordance( edit, gloss.id() );
    connect(edit, SIGNAL(destroyed(QObject*)), mConcordance, SLOT(removeGlossFromLingEditConcordance(QObject*)));
    connect(edit, SIGNAL(stringChanged(TextBit,LingEdit*)), mConcordance, SLOT(updateGlossLingEditConcordance(TextBit,LingEdit*)));

    // update the gloss item
    connect(edit,SIGNAL(stringChanged(TextBit,LingEdit*)), mGlossItem, SLOT(setGloss(TextBit)) );

    return edit;
}

LingEdit* WordDisplayWidget::addTextFormLine( const InterlinearItemType & glossLine )
{
    TextBit textForm = mGlossItem->textForm( glossLine.writingSystem() );
    LingEdit *edit = new LingEdit(  textForm , this);
    edit->matchTextAlignmentTo( mGlossLines.first().writingSystem().layoutDirection() );

    mTextFormEdits.insert(glossLine.writingSystem(), edit);

    // update the concordance
    mConcordance->updateTextFormLingEditConcordance( edit, textForm.id() );
    connect( edit, SIGNAL(destroyed(QObject*)), mConcordance, SLOT(removeTextFormFromLingEditConcordance(QObject*)));
    connect( edit, SIGNAL(stringChanged(TextBit,LingEdit*)), mConcordance, SLOT(updateTextFormLingEditConcordance(TextBit,LingEdit*)));

    // update the gloss item
    connect(edit,SIGNAL(stringChanged(TextBit,LingEdit*)), mGlossItem, SLOT(setTextForm(TextBit)) );

    return edit;
}

ImmutableLabel* WordDisplayWidget::addImmutableTextFormLine( const InterlinearItemType & glossLine, bool technicolor )
{
    TextBit bit = mGlossItem->textForm( glossLine.writingSystem() );

    ImmutableLabel *immutableLabel = new ImmutableLabel( bit, technicolor , this);
    immutableLabel->setAlignment( mGlossLines.first().writingSystem().layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight );
    immutableLabel->setCandidateNumber(mGlossItem->candidateNumber());
    immutableLabel->setApprovalStatus(mGlossItem->approvalStatus());

    mImmutableLines.insert( glossLine.writingSystem() , immutableLabel );

    connect( mGlossItem, SIGNAL(approvalStatusChanged(GlossItem::ApprovalStatus)), immutableLabel, SLOT(setApprovalStatus(GlossItem::ApprovalStatus)) );
    connect( mGlossItem, SIGNAL(candidateNumberChanged(GlossItem::CandidateNumber)), immutableLabel, SLOT(setCandidateNumber(GlossItem::CandidateNumber)) );

    mConcordance->updateTextForImmutableLabelConcordance( immutableLabel, bit.id() );
    connect( immutableLabel, SIGNAL(destroyed(QObject*)), mConcordance, SLOT(removeTextFormFromImmutableLabelConcordance(QObject*)));

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

    mConcordance->updateGlossImmutableLabelConcordance( immutableLabel, bit.id() );
    connect( immutableLabel, SIGNAL(destroyed(QObject*)), mConcordance, SLOT(removeGlossFromImmutableLabelConcordance(QObject*)));

    connect( mGlossItem, SIGNAL(approvalStatusChanged(GlossItem::ApprovalStatus)), immutableLabel, SLOT(setApprovalStatus(GlossItem::ApprovalStatus)) );
    connect( mGlossItem, SIGNAL(candidateNumberChanged(GlossItem::CandidateNumber)), immutableLabel, SLOT(setCandidateNumber(GlossItem::CandidateNumber)) );

    return immutableLabel;
}

AnalysisWidget* WordDisplayWidget::addAnalysisWidget( const InterlinearItemType & glossLine )
{
    AnalysisWidget *analysisWidget = new AnalysisWidget(mGlossItem, glossLine.writingSystem(), mDbAdapter, this);
    mAnalysisWidgets.insert( glossLine.writingSystem(), analysisWidget );

    connect( mGlossItem, SIGNAL(morphologicalAnalysisChanged(MorphologicalAnalysis)), analysisWidget, SLOT(createInitializedLayout(MorphologicalAnalysis)));
    connect( analysisWidget, SIGNAL(morphologicalAnalysisChanged(MorphologicalAnalysis)), mConcordance, SLOT(updateGlossItemMorphologicalAnalysis(MorphologicalAnalysis)) );

    return analysisWidget;
}

void WordDisplayWidget::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    menu.addAction(tr("Edit baseline text"), this, SLOT(editBaselineText()));
    menu.addAction(tr("Change to two words"), this, SLOT(changeToTwoWords()));
    menu.addAction(tr("Merge with next"), this, SLOT(mergeWithNext()));
    menu.addAction(tr("Merge with previous"), this, SLOT(mergeWithPrevious()));

    menu.addSeparator();

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
        switch( mGlossLines.at(i).type() )
        {
        case InterlinearItemType::ImmutableText:
        case InterlinearItemType::ImmutableGloss:
            break;
        case InterlinearItemType::Text:
            addTextFormSubmenu( &menu , mGlossLines.at(i).writingSystem() );
            break;
        case InterlinearItemType::Gloss:
            addGlossSubmenu( &menu , mGlossLines.at(i).writingSystem() );
            break;
        case InterlinearItemType::Analysis:
            break;
        case InterlinearItemType::Null:
            break;
        }
    }

    menu.addSeparator();

    menu.addAction(tr("Database report"), this, SLOT(displayDatabaseReport()) );
    addSearchSubmenu(&menu);

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

    submenu->addAction(tr("New interpretation..."),this,SLOT(newInterpretation()));
    submenu->addAction(tr("Link to other interpretation..."),this,SLOT(otherInterpretation()));

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
    QAction *action = new QAction(tr("New text form..."),menu);
    action->setData( writingSystem.id() );
    oneOffgroup->addAction(action);
    connect( oneOffgroup, SIGNAL(triggered(QAction*)), this, SLOT(newTextForm(QAction*)) );
    submenu->addAction(action);

    QActionGroup *oneOffgroup2 = new QActionGroup(menu);
    action = new QAction(tr("Copy from baseline text..."),menu);
    action->setData( writingSystem.id() );
    oneOffgroup2->addAction(action);
    connect( oneOffgroup2, SIGNAL(triggered(QAction*)), this, SLOT(copyTextFormFromBaseline(QAction*)) );
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

void WordDisplayWidget::copyGlossFromBaseline(QAction *action)
{
    qlonglong wsId = action->data().toLongLong();
    WritingSystem ws = mDbAdapter->writingSystem(wsId);

    TextBit bit = mGlossEdits[ws]->textBit();
    bit.setText( mGlossItem->textForm(mGlossItem->baselineWritingSystem()).text() );
    if( bit.id() == -1 )
    {
        qlonglong id = mDbAdapter->newGloss( mGlossItem->id() , bit );
        bit.setId( id );
    }

    mDbAdapter->updateInterpretationGloss( bit );
    mGlossEdits[ws]->setTextBit( bit );
}

void WordDisplayWidget::copyTextFormFromBaseline(QAction *action)
{
    qlonglong wsId = action->data().toLongLong();
    WritingSystem ws = mDbAdapter->writingSystem(wsId);

    TextBit bit = mTextFormEdits[ws]->textBit();
    bit.setText( mGlossItem->textForm(mGlossItem->baselineWritingSystem()).text() );
    if( bit.id() == -1 )
    {
        qlonglong id = mDbAdapter->newTextForm( mGlossItem->id() , bit );
        bit.setId( id );
    }

    mDbAdapter->updateInterpretationTextForm( bit );
    mTextFormEdits[ws]->setTextBit( bit );
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
                // TODO how does one fill the data here? what does this function even do?
                break;
            case InterlinearItemType::Null:
                break;
            }
        }
    }
}

void WordDisplayWidget::mouseDoubleClickEvent ( QMouseEvent * event )
{
    mGlossItem->toggleApproval();
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

void WordDisplayWidget::otherInterpretation()
{
    InterpretationSearchDialog dialog( mDbAdapter, this );
    if( dialog.exec() == QDialog::Accepted )
    {
        qlonglong id = dialog.selectionId();
        mDbAdapter->newTextForm( id , mGlossItem->baselineText() );
        mGlossItem->setInterpretation(id, true); // true because the user hadn't had a chance to specify more particularly yet
        fillData();
    }
}

GlossItem* WordDisplayWidget::glossItem()
{
    return mGlossItem;
}

void WordDisplayWidget::refreshMorphologicalAnalysis(const WritingSystem & ws)
{
    //    if( mAnalysisWidgets.contains(ws) )
    //    {
    //        mGlossItem->setMorphologicalAnalysisFromDatabase( ws );
    //        mAnalysisWidgets[ws]->createInitializedLayout();
    //    }
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
        mGlossItem->resetBaselineText( dialog.textBit() );
}

void WordDisplayWidget::changeToTwoWords()
{
    GenericTextInputDialog dialog( mGlossItem->baselineText() , this );
    dialog.setWindowTitle(tr("Split the word with a space"));
    if( dialog.exec() == QDialog::Accepted )
    {
        QStringList items = dialog.text().split(QRegExp("\\s"));
        if( items.count() == 2 )
            emit splitWidgetInTwo( mGlossItem, TextBit(items.at(0), mGlossItem->baselineWritingSystem()), TextBit(items.at(1), mGlossItem->baselineWritingSystem()) );
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
