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

#include <QtGui>
#include <QtDebug>
#include <QActionGroup>

WordDisplayWidget::WordDisplayWidget( GlossItem *item, Qt::Alignment alignment, const QList<InterlinearItemType> & lines, InterlinearDisplayWidget *ildw, DatabaseAdapter *dbAdapter)
{
    mDbAdapter = dbAdapter;
    mGlossItem = item;
    mAlignment = alignment;
    mInterlinearDisplayWidget = ildw;

    mGlossLines = lines;

    setMaximumWidth(100);

    setupLayout();

    setMinimumSize(128,20);

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));

    connect( mGlossItem, SIGNAL(fieldsChanged()), this, SLOT(fillData()) );

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
    mAnalysisWidgets.clear();

    QLabel *immutableLabel;
    LingEdit *lineWidget;
    AnalysisWidget *analysisWidget;

    for(int i=0; i<mGlossLines.count(); i++)
    {
        switch( mGlossLines.at(i).type() )
        {
        case InterlinearItemType::Immutable:
            immutableLabel = addImmutableLine( mGlossLines.at(i) , i == 0 );
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
        }
    }
}

LingEdit* WordDisplayWidget::addGlossLine( const InterlinearItemType & glossLine )
{
    LingEdit *edit = new LingEdit( mGlossItem->gloss( glossLine.writingSystem() ) , this);
    edit->matchTextAlignmentTo( glossLine.writingSystem().layoutDirection() );

    mGlossEdits.insert(glossLine.writingSystem(), edit);

    connect(this, SIGNAL(glossIdChanged(LingEdit*,qlonglong)), edit, SLOT(setId(LingEdit*,qlonglong)));
    connect(edit,SIGNAL(stringChanged(TextBit)), mGlossItem, SLOT(setGloss(TextBit)) );
    connect(edit, SIGNAL(stringChanged(TextBit)), mInterlinearDisplayWidget, SLOT(updateGloss(TextBit)));
    connect(edit, SIGNAL(beingDestroyed(LingEdit*)), mInterlinearDisplayWidget, SLOT(removeGlossFromConcordance(LingEdit*)));

    return edit;
}

LingEdit* WordDisplayWidget::addTextFormLine( const InterlinearItemType & glossLine )
{
    LingEdit *edit = new LingEdit(  mGlossItem->textForm( glossLine.writingSystem() ) , this);
    edit->matchTextAlignmentTo( mGlossLines.first().writingSystem().layoutDirection() );

    mTextFormEdits.insert(glossLine.writingSystem(), edit);

    connect(this, SIGNAL(textFormIdChanged(LingEdit*,qlonglong)), edit, SLOT(setId(LingEdit*,qlonglong)));
    connect(edit,SIGNAL(stringChanged(TextBit)), mGlossItem, SLOT(setTextForm(TextBit)) );
    connect(edit, SIGNAL(stringChanged(TextBit)), mInterlinearDisplayWidget, SLOT(updateText(TextBit)));
    connect(edit, SIGNAL(beingDestroyed(LingEdit*)), mInterlinearDisplayWidget, SLOT(removeTextFormFromConcordance(LingEdit*)));

    return edit;
}

ImmutableLabel* WordDisplayWidget::addImmutableLine( const InterlinearItemType & glossLine, bool technicolor )
{
    TextBit bit("",glossLine.writingSystem());
    if( mGlossItem->textForms()->contains( glossLine.writingSystem() ))
        bit = mGlossItem->textForm( glossLine.writingSystem() );
    else if( mGlossItem->glosses()->contains( glossLine.writingSystem() ) )
        bit = mGlossItem->gloss( glossLine.writingSystem() );

    ImmutableLabel *immutableLabel = new ImmutableLabel( bit, technicolor , this);
    immutableLabel->setAlignment( mGlossLines.first().writingSystem().layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight );
    immutableLabel->setCandidateNumber(mGlossItem->candidateNumber());
    immutableLabel->setApprovalStatus(mGlossItem->approvalStatus());

    connect( mGlossItem, SIGNAL(approvalStatusChanged(GlossItem::ApprovalStatus)), immutableLabel, SLOT(setApprovalStatus(GlossItem::ApprovalStatus)) );
    connect( mGlossItem, SIGNAL(candidateNumberChanged(GlossItem::CandidateNumber)), immutableLabel, SLOT(setCandidateNumber(GlossItem::CandidateNumber)) );

    return immutableLabel;
}

AnalysisWidget* WordDisplayWidget::addAnalysisWidget( const InterlinearItemType & glossLine )
{
    AnalysisWidget *analysisWidget = new AnalysisWidget(mGlossItem, glossLine.writingSystem(), mDbAdapter, this);
    mAnalysisWidgets.insert( glossLine.writingSystem(), analysisWidget );

    connect( analysisWidget, SIGNAL(morphologicalAnalysisChanged(qlonglong)), this, SIGNAL(morphologicalAnalysisChanged(qlonglong)) );

    return analysisWidget;
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
        switch( mGlossLines.at(i).type() )
        {
        case InterlinearItemType::Immutable:
            break;
        case InterlinearItemType::Text:
            addTextFormSubmenu( &menu , mGlossLines.at(i).writingSystem() );
            break;
        case InterlinearItemType::Gloss:
            addGlossSubmenu( &menu , mGlossLines.at(i).writingSystem() );
            break;
        case InterlinearItemType::Analysis:
            break;
        }
    }

    menu.addSeparator();
    menu.addAction(tr("Database report"), this, SLOT(displayDatabaseReport()) );

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

void WordDisplayWidget::newInterpretation()
{
    emit alternateInterpretationAvailableFor(mGlossItem->id());
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

    mDbAdapter->updateInterpretationGloss( bit );
    mGlossEdits[ws]->setTextBit( bit );
}

void WordDisplayWidget::copyTextFormFromBaseline(QAction *action)
{
    qlonglong wsId = action->data().toLongLong();
    WritingSystem ws = mDbAdapter->writingSystem(wsId);

    TextBit bit = mTextFormEdits[ws]->textBit();
    bit.setText( mGlossItem->textForm(mGlossItem->baselineWritingSystem()).text() );

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
        emit glossIdChanged( mGlossEdits.value(ws) , id );
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
        emit textFormIdChanged( mTextFormEdits.value(ws) , id );
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
            case InterlinearItemType::Immutable:
            case InterlinearItemType::Analysis:
                // TODO how does one fill the data here? what does this function even do?
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

    //    QHashIterator<WritingSystem, AnalysisWidget*> iter2(mAnalysisWidgets);
    //    while(iter2.hasNext())
    //    {
    //        iter2.next();
    //        // TODO connect this signal to something
    //        emit analysisChanged( iter2.value() , mGlossItem->morphologicalAnalysis( iter2.key() )->id() );
    //    }

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
    if( mAnalysisWidgets.contains(ws) )
    {
        mGlossItem->setMorphologicalAnalysisFromDatabase( ws );
        mAnalysisWidgets[ws]->createInitializedLayout();
    }
}

void WordDisplayWidget::displayDatabaseReport()
{
    QString reportString;

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
