#include "analysiswidget.h"
#include "createanalysisdialog.h"

#include <QtWidgets>

#include "glossitem.h"
#include "chooselexicalentriesdialog.h"
#include "createlexicalentrydialog.h"
#include "databaseadapter.h"
#include "immutablelabel.h"
#include "lexicalentrysearchdialog.h"

AnalysisWidget::AnalysisWidget(GlossItem *glossItem, const WritingSystem & analysisWs, const DatabaseAdapter *dbAdapter, QWidget *parent) :
        QWidget(parent)
{
    mGlossItem = glossItem;
    mWritingSystem = analysisWs;
    mDbAdapter = dbAdapter;

    mLayout = new QVBoxLayout;
    mLayout->setSpacing(0);
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

void AnalysisWidget::createUninitializedLayout()
{
    clearWidgetsFromLayout();

    QPushButton *createMle = new QPushButton(tr("Monomorphemic"), this);
    createMle->setToolTip(tr("Create monomorphemic lexical entry"));
    createMle->setFlat(true);
    createMle->setStyleSheet("QPushButton { color: blue; text-decoration: underline; padding: 0px; padding-bottom: 6px;  padding-top: 6px; }");
    mLayout->addWidget(createMle);
    connect(createMle, SIGNAL(clicked()), this, SLOT(createMonomorphemicLexicalEntry()));

    QPushButton *analyze = new QPushButton(tr("Polymorphemic"), this);
    analyze->setFlat(true);
    analyze->setStyleSheet("QPushButton { color: blue; text-decoration: underline; padding: 0px; padding-bottom: 6px; padding-top: 6px; }");
    mLayout->addWidget(analyze);
    connect(analyze, SIGNAL(clicked()), this, SLOT(enterAnalysis()));
}

void AnalysisWidget::createInitializedLayout(const MorphologicalAnalysis * analysis)
{
    clearWidgetsFromLayout();

    mLayout->addWidget( new ImmutableLabel( TextBit( analysis->baselineSummary() , mWritingSystem ) , false, this ) );

    QList<WritingSystem> glossLines = mDbAdapter->lexicalEntryGlossFields();
    for(int i=0; i<glossLines.count(); i++)
        mLayout->addWidget( new ImmutableLabel( TextBit( analysis->glossSummary(glossLines.at(i)), mWritingSystem ), false, this ) );
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
        AllomorphIterator iter = analysis->allomorphIterator();
        QActionGroup * group = new QActionGroup(this);
        while(iter.hasNext())
        {
            Allomorph allomorph = iter.next();
            QAction * action = new QAction(tr("Edit %1").arg( allomorph.text() ), this);
            action->setData( allomorph.id() );
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
    qlonglong lexicalEntryId = mDbAdapter->lexicalEntryIdFromAllomorph(allomorphId);

    if( lexicalEntryId == -1 )
    {
        qWarning() << "No lexical id for that allomorph";
        return;
    }

    CreateLexicalEntryDialog dialog( lexicalEntryId, mGlossItem, mDbAdapter, this);
    dialog.exec();
}

void AnalysisWidget::enterAnalysis()
{
    CreateAnalysisDialog dialog( textBit() );
    if( dialog.exec() == QDialog::Accepted )
    {
        ChooseLexicalEntriesDialog leDialog( TextBit(dialog.analysisString(), textBit().writingSystem(), textBit().id() ), mGlossItem,  mDbAdapter , this);
        connect( &leDialog, SIGNAL(rejected()), this, SLOT(enterAnalysis()) );
        if( leDialog.exec() == QDialog::Accepted )
        {
            createInitializedLayout( leDialog.morphologicalAnalysis() );
            emit morphologicalAnalysisChanged( leDialog.morphologicalAnalysis() );
        }
    }
}

void AnalysisWidget::createMonomorphemicLexicalEntry()
{
    qlonglong lexicalEntryId;

    lexicalEntryId = selectCandidateLexicalEntry();

    Allomorph allomorph( -1, textBit() , Allomorph::typeFromFormattedString( textBit().text() ) );
    if( lexicalEntryId == -1 )
    {
        CreateLexicalEntryDialog dialog( &allomorph, true, mGlossItem, mDbAdapter, this);
        connect( &dialog, SIGNAL(linkToOther()), this, SLOT(linkToOther()) );
        if( dialog.exec() == QDialog::Accepted )
            lexicalEntryId = dialog.lexicalEntryId();
    }

    if( lexicalEntryId != -1 )
    {
        qlonglong allomorphId = mDbAdapter->addAllomorph( textBit() , lexicalEntryId );
        allomorph = mDbAdapter->allomorphFromId(allomorphId);

        MorphologicalAnalysis * analysis = new MorphologicalAnalysis( textBit() );
        analysis->addAllomorph( allomorph );
        mDbAdapter->setMorphologicalAnalysis( textBit().id(), analysis );

        createInitializedLayout( analysis );
        emit morphologicalAnalysisChanged( analysis );
    }
}

qlonglong AnalysisWidget::selectCandidateLexicalEntry()
{
    QStringList candidateItems;
    QList<qlonglong> indices;
    QHash<qlonglong,QString> candidates = mDbAdapter->getLexicalEntryCandidates( textBit() , Allomorph::getTypeString(Allomorph::Stem) );

    if(candidates.isEmpty())
        return -1;

    QHashIterator<qlonglong,QString> iter(candidates);
    while(iter.hasNext())
    {
        iter.next();
        candidateItems << iter.value();
        indices << iter.key();
    }
    bool ok;
    QString choice = QInputDialog::getItem(this, tr("Select candidate"), tr("Select an existing lexical item, or cancel to create a new one."), candidateItems, 0, false, &ok );
    if( ok )
        return indices.at( candidateItems.indexOf( choice ) );
    else
        return -1;
}

void AnalysisWidget::clearWidgetsFromLayout()
{
    if( mLayout->count() == 0 )
        return;
    QLayoutItem * item;
    while( ( item = mLayout->takeAt(0) ) != 0 )
    {
        item->widget()->deleteLater();;
        delete item;
    }
}

void AnalysisWidget::linkToOther()
{
    LexicalEntrySearchDialog dialog(mDbAdapter, this);
    if( dialog.exec() == QDialog::Accepted )
    {
        qlonglong lexicalEntryId = dialog.lexicalEntryId();
        if( lexicalEntryId != -1 )
        {
            qlonglong allomorphId = mDbAdapter->addAllomorph( textBit() , lexicalEntryId );
            MorphologicalAnalysis * monomorphemic = new MorphologicalAnalysis( textBit() );
            monomorphemic->addAllomorph( mDbAdapter->allomorphFromId(allomorphId) );
            emit morphologicalAnalysisChanged( monomorphemic );
        }
    }
}
