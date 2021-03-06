#include "analysiswidget.h"
#include "createanalysisdialog.h"

#include <QtWidgets>

#include "glossitem.h"
#include "chooselexicalentriesdialog.h"
#include "createlexicalentrydialog.h"
#include "databaseadapter.h"
#include "immutablelabel.h"
#include "lexicalentrysearchdialog.h"
#include "project.h"

AnalysisWidget::AnalysisWidget(GlossItem *glossItem, const WritingSystem & analysisWs, Qt::LayoutDirection layoutDirection, Project *project, QWidget *parent) :
        QWidget(parent), mProject(project), mGlossItem(glossItem), mWritingSystem(analysisWs), mLayoutDirection(layoutDirection), mDbAdapter(mProject->dbAdapter())
{
    mLayout = new QVBoxLayout;
    mLayout->setSpacing(0);
    mLayout->setContentsMargins(0,0,0,0);
    setLayout(mLayout);

    setupLayout();
}

void AnalysisWidget::setupLayout()
{
    if( mGlossItem->morphologicalAnalysis(mWritingSystem)->isEmpty() )
        createUninitializedLayout();
    else
        createInitializedLayout( mGlossItem->morphologicalAnalysis(mWritingSystem) );
}

void AnalysisWidget::allomorphDoubleClick(TextBit &bit)
{
    int allomorphIndex = static_cast<int>(bit.id());
    editLexicalEntry( mGlossItem->morphologicalAnalysis(mWritingSystem)->allomorph(allomorphIndex)->id() );
}

void AnalysisWidget::createUninitializedLayout()
{
    clearWidgetsFromLayout(mLayout);

    QPushButton *createMle = new QPushButton(tr("Monomorphemic"), this);
    createMle->setToolTip(tr("Create monomorphemic lexical entry"));
    createMle->setFlat(true);
    createMle->setStyleSheet("QPushButton { color: blue; text-decoration: underline; padding: 0px; padding-bottom: 6px;  padding-top: 6px; }");
    mLayout->addWidget(createMle);
    connect(createMle, SIGNAL(clicked()), this, SLOT(createMonomorphemicLexicalEntry()));

    QPushButton *analyze = new QPushButton(tr("Polymorphemic"), this);
    createMle->setToolTip(tr("Create polymorphemic lexical entry"));
    analyze->setFlat(true);
    analyze->setStyleSheet("QPushButton { color: blue; text-decoration: underline; padding: 0px; padding-bottom: 6px; padding-top: 6px; }");
    mLayout->addWidget(analyze);
    connect(analyze, SIGNAL(clicked()), this, SLOT(enterAnalysis()));

    QPushButton *createQMle = new QPushButton(tr("Quick Mono"), this);
    createQMle->setToolTip(tr("Create monomorphemic lexical entry without a chance to edit"));
    createQMle->setFlat(true);
    createQMle->setStyleSheet("QPushButton { color: blue; text-decoration: underline; padding: 0px; padding-bottom: 6px;  padding-top: 6px; }");
    mLayout->addWidget(createQMle);
    connect(createQMle, SIGNAL(clicked()), this, SLOT(createQuickMonomorphemicLexicalEntry()));

    QList< QPair<qlonglong,QString> > candidates = mDbAdapter->getLexicalEntryCandidates( textBit() , Allomorph::getTypeString(Allomorph::Stem), false );
    if( candidates.count() == 1 )
    {
        QPushButton *createGMle = new QPushButton( candidates.first().second, this);
        createGMle->setToolTip(tr("Link this word to the indicated element"));
        createGMle->setFlat(true);
        createGMle->setStyleSheet("QPushButton { color: blue; text-decoration: underline; padding: 0px; padding-bottom: 6px;  padding-top: 6px; }");
        mLayout->addWidget(createGMle);
        connect(createGMle, SIGNAL(clicked()), this, SLOT(createGuessedLexicalEntry()));
    }
}

void AnalysisWidget::createInitializedLayout(const MorphologicalAnalysis * analysis)
{
    clearWidgetsFromLayout(mLayout);

    /// add the baseline representation
    QHBoxLayout * baseLayout = new QHBoxLayout;
    baseLayout->setContentsMargins(0,0,0,0);
    if( mLayoutDirection == Qt::RightToLeft )
    {
        baseLayout->addSpacerItem( new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum) );
    }
    for(int i=0; i< analysis->allomorphCount(); i++)
    {
        ImmutableLabel * label = new ImmutableLabel( analysis->baselineText(i) , false );
        label->matchTextAlignmentTo( mLayoutDirection );
        baseLayout->addWidget( label );
        connect( label, SIGNAL(doubleClick(TextBit&)), this, SLOT(allomorphDoubleClick(TextBit&)) );
    }
    if( mLayoutDirection == Qt::LeftToRight )
    {
        baseLayout->addSpacerItem( new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum) );
    }
    mLayout->addLayout(baseLayout);

    /// add each of the gloss lines
    const QList<WritingSystem> *glossLines = mProject->lexicalEntryGlossFields();
    for(int i=0; i<glossLines->count(); i++)
    {
        QHBoxLayout * hlayout = new QHBoxLayout;
        hlayout->setContentsMargins(0,0,0,0);
        if( mLayoutDirection == Qt::RightToLeft )
        {
            hlayout->addSpacerItem( new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum) );
        }
        for(int j=0; j< analysis->allomorphCount(); j++)
        {
            ImmutableLabel * label = new ImmutableLabel( analysis->gloss(j, glossLines->at(i)) , false );
            label->matchTextAlignmentTo( mLayoutDirection );
            hlayout->addWidget( label );
            connect( label, SIGNAL(doubleClick(TextBit&)), this, SLOT(allomorphDoubleClick(TextBit&)) );
        }
        if( mLayoutDirection == Qt::LeftToRight )
        {
            hlayout->addSpacerItem( new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum) );
        }
        mLayout->addLayout(hlayout);
    }
    for( int i=0; i<analysis->allomorphCount(); i++ )
    {
        connect( analysis->allomorph(i), SIGNAL(glossesChanged(Allomorph*)), this, SLOT(setupLayout()) );
    }
}

void AnalysisWidget::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu * menu = new QMenu(this);
    menu->addAction(tr("Create monomorphemic analysis"), this, SLOT(createMonomorphemicLexicalEntry()));
    menu->addAction(tr("Create polymorphemic analysis"), this, SLOT(enterAnalysis()));

    menu->addSeparator();
    QAction * duplicate = menu->addAction(tr("Duplicate interpretation"), this, SIGNAL(requestAlternateInterpretation()));
    connect( duplicate, SIGNAL(triggered()), this, SLOT(enterAnalysis()));

    MorphologicalAnalysis * analysis = mGlossItem->morphologicalAnalysis(mWritingSystem);
    if( !analysis->isEmpty() )
    {
        menu->addSeparator();
        AllomorphPointerIterator iter = analysis->allomorphIterator();
        QActionGroup * group = new QActionGroup(this);
        while(iter.hasNext())
        {
            const Allomorph *allomorph = iter.next();
            QAction * action = new QAction(tr("Edit %1").arg( allomorph->text() ), this);
            action->setData( allomorph->id() );
            menu->addAction(action);
            group->addAction(action);
        }
        connect( group, SIGNAL(triggered(QAction*)) , this, SLOT(editLexicalEntry(QAction*)) );
    }

    menu->exec(event->globalPos());
}

void AnalysisWidget::editLexicalEntry(QAction * action)
{
    qlonglong allomorphId = action->data().toLongLong();
    editLexicalEntry( allomorphId );
}

void AnalysisWidget::editLexicalEntry(qlonglong allomorphId)
{
    qlonglong lexicalEntryId = mDbAdapter->lexicalEntryIdFromAllomorph(allomorphId);

    if( lexicalEntryId == -1 )
    {
        qWarning() << "No lexical id for that allomorph";
        return;
    }

    CreateLexicalEntryDialog dialog( lexicalEntryId, false, mGlossItem, mProject, this);
    if( dialog.exec() )
    {
        Allomorph * a = mGlossItem->morphologicalAnalysis(mWritingSystem)->allomorphFromId( allomorphId );
        if( a != nullptr )
        {
            a->setGlosses( mDbAdapter->lexicalEntryGlossFormsForAllomorph( allomorphId ) );
        }
        setupLayout();
    }
}

void AnalysisWidget::enterAnalysis()
{
    CreateAnalysisDialog dialog( mDbAdapter, textBit() );
    if( dialog.exec() == QDialog::Accepted )
    {
        ChooseLexicalEntriesDialog leDialog( TextBit(dialog.analysisString(), textBit().writingSystem(), textBit().id() ), mGlossItem,  mProject , this);
        connect( &leDialog, SIGNAL(rejected()), this, SLOT(enterAnalysis()) );
        if( leDialog.exec() == QDialog::Accepted )
        {
            MorphologicalAnalysis * analysis = leDialog.morphologicalAnalysis();
            createInitializedLayout( analysis );
            /// emit 0 so that the concordance actually does change this gloss item
            emit morphologicalAnalysisChanged( nullptr, analysis );
        }
    }
}

void AnalysisWidget::createAndDisplayAnalysis(qlonglong lexicalEntryId)
{
    if( lexicalEntryId != -1 ) /// if we have a valid lexical entry
    {
        qlonglong allomorphId = mDbAdapter->addAllomorph( textBit() , lexicalEntryId );
        Allomorph *allomorph = mDbAdapter->allomorphFromId(allomorphId);

        MorphologicalAnalysis * analysis = new MorphologicalAnalysis( textBit(), mProject->concordance() );
        analysis->addAllomorph( allomorph );
        mDbAdapter->setMorphologicalAnalysis( textBit().id(), analysis );

        createInitializedLayout( analysis );
        /// emit 0 so that the concordance actually does change this gloss item
        emit morphologicalAnalysisChanged( nullptr, analysis );
    }
}

void AnalysisWidget::createMonomorphemicLexicalEntry()
{
    qlonglong lexicalEntryId = selectCandidateLexicalEntry();
    if( lexicalEntryId == -1 )
    {
        CreateLexicalEntryDialog dialog( textBit(), true, true, mGlossItem, mProject, this);
        connect( &dialog, SIGNAL(linkToOther()), this, SLOT(linkToOther()) );
        if( dialog.exec() == QDialog::Accepted )
            lexicalEntryId = dialog.lexicalEntryId();
    }
    if( lexicalEntryId != -1 )
    {
        createAndDisplayAnalysis(lexicalEntryId);
    }
}

void AnalysisWidget::createQuickMonomorphemicLexicalEntry()
{
    qlonglong lexicalEntryId = selectCandidateLexicalEntry();
    if( lexicalEntryId == -1 ) /// if the user has not just chosen a different lexical entry
    {
        lexicalEntryId = mDbAdapter->addLexicalEntry( "", Allomorph::Stem, mGlossItem->glosses()->values(), mGlossItem->textForms()->values(), QStringList() );
    }
    createAndDisplayAnalysis(lexicalEntryId);
}

void AnalysisWidget::createGuessedLexicalEntry()
{
    QList< QPair<qlonglong,QString> > candidates = mDbAdapter->getLexicalEntryCandidates( textBit() , Allomorph::getTypeString(Allomorph::Stem) );
    if( candidates.count() == 1 )
    {
        createAndDisplayAnalysis(candidates.first().first);
    }
}

qlonglong AnalysisWidget::selectCandidateLexicalEntry()
{
    QStringList candidateItems;
    QList<qlonglong> indices;
    QList< QPair<qlonglong,QString> > candidates = mDbAdapter->getLexicalEntryCandidates( textBit() , Allomorph::getTypeString(Allomorph::Stem) );

    if(candidates.isEmpty())
        return -1;

    QListIterator< QPair<qlonglong,QString> > iter(candidates);
    while(iter.hasNext())
    {
        QPair<qlonglong,QString> pair = iter.next();
        candidateItems << pair.second;
        indices << pair.first;
    }
    bool ok;
    QString choice = QInputDialog::getItem(this, tr("Select candidate"), tr("Select an existing lexical item, or cancel to create a new one."), candidateItems, 0, false, &ok );
    if( ok )
        return indices.at( candidateItems.indexOf( choice ) );
    else
        return -1;
}

void AnalysisWidget::clearWidgetsFromLayout(QLayout * layout)
{
    if( layout->count() == 0 )
        return;
    QLayoutItem * item;
    while( ( item = layout->takeAt(0) ) != nullptr )
    {
        if( item->widget() != nullptr )
        {
            item->widget()->deleteLater();
        }
        else if ( item->layout() != nullptr )
        {
            clearWidgetsFromLayout(item->layout());
            item->layout()->deleteLater();
        }
        delete item;
    }
}

void AnalysisWidget::linkToOther()
{
    LexicalEntrySearchDialog dialog(mProject    , this);
    if( dialog.exec() == QDialog::Accepted )
    {
        qlonglong lexicalEntryId = dialog.lexicalEntryId();
        if( lexicalEntryId != -1 )
        {
            qlonglong allomorphId = mDbAdapter->addAllomorph( textBit() , lexicalEntryId );
            MorphologicalAnalysis * monomorphemic = new MorphologicalAnalysis( textBit(), mProject->concordance() );
            monomorphemic->addAllomorph( mDbAdapter->allomorphFromId(allomorphId) );
            /// emit 0 so that the concordance actually does change this gloss item
            emit morphologicalAnalysisChanged( nullptr, monomorphemic );
        }
    }
}
