#include "analysiswidget.h"
#include "createanalysisdialog.h"

#include <QtGui>

#include "glossitem.h"
#include "chooselexicalentriesdialog.h"
#include "createlexicalentrydialog.h"
#include "databaseadapter.h"
#include "immutablelabel.h"

AnalysisWidget::AnalysisWidget(GlossItem *glossItem, const WritingSystem & analysisWs, DatabaseAdapter *dbAdapter, QWidget *parent) :
    QWidget(parent)
{
    mGlossItem = glossItem;
    mWritingSystem = analysisWs;
    mDbAdapter = dbAdapter;
    setupLayout();
}

void AnalysisWidget::setupLayout()
{
    mLayout = new QVBoxLayout;
    setLayout(mLayout);

    if( mGlossItem->morphologicalAnalysis(mWritingSystem)->isEmpty() )
        createUninitializedLayout();
    else
        createInitializedLayout();
}

void AnalysisWidget::createUninitializedLayout()
{
    clearWidgetsFromLayout();

    QPushButton *analyze = new QPushButton(tr("Analyze"), this);
    analyze->setFlat(true);
    analyze->setStyleSheet("QPushButton { color: blue; text-decoration: underline; padding: 0px; }");
    mLayout->addWidget(analyze);
    connect(analyze, SIGNAL(clicked()), this, SLOT(enterAnalysis()));

    QPushButton *createMle = new QPushButton(tr("Create MLE"), this);
    createMle->setToolTip(tr("Create monomorphemic lexical entry"));
    createMle->setFlat(true);
    createMle->setStyleSheet("QPushButton { color: blue; text-decoration: underline; padding: 0px; }");
    mLayout->addWidget(createMle);
    connect(createMle, SIGNAL(clicked()), this, SLOT(createMonomorphemicLexicalEntry()));
}

void AnalysisWidget::createInitializedLayout()
{
    clearWidgetsFromLayout();

    mLayout->addWidget( new ImmutableLabel( TextBit( mGlossItem->morphologicalAnalysis(mWritingSystem)->baselineSummary() , mWritingSystem ) , false, this ) );

    QList<WritingSystem> glossLines = mDbAdapter->lexicalEntryGlosses();
    for(int i=0; i<glossLines.count(); i++)
        mLayout->addWidget( new ImmutableLabel( TextBit( mGlossItem->morphologicalAnalysis(mWritingSystem)->glossSummary(glossLines.at(i)), mWritingSystem ), false, this ) );
}

void AnalysisWidget::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);
    menu.addAction(tr("Analyze"), this, SLOT(enterAnalysis()));
    menu.addAction(tr("Create MLE"), this, SLOT(createMonomorphemicLexicalEntry()));
    menu.exec(event->globalPos());
}

void AnalysisWidget::enterAnalysis()
{
    TextBit textForm = mGlossItem->textForm( mWritingSystem );
    CreateAnalysisDialog dialog( textForm );
    if( dialog.exec() == QDialog::Accepted )
    {
        ChooseLexicalEntriesDialog leDialog( TextBit(dialog.analysisString(), textForm.writingSystem(), textForm.id() ), mGlossItem,  mDbAdapter , this);
        if( leDialog.exec() == QDialog::Accepted )
        {
            createInitializedLayout();
            emit morphologicalAnalysisChanged( mGlossItem->textForm(mWritingSystem).id() );
        }
    }
}

void AnalysisWidget::createMonomorphemicLexicalEntry()
{
    TextBit textForm = mGlossItem->textForm(mWritingSystem);
    CreateLexicalEntryDialog dialog( textForm, true, mGlossItem, mDbAdapter, this);
    if( dialog.exec() == QDialog::Accepted )
    {
        qlonglong lexicalEntryId = dialog.id();
        if( lexicalEntryId != -1 )
        {
            qlonglong allomorphId = mDbAdapter->addAllomorph( textForm , lexicalEntryId );

            Allomorph allomorph = Allomorph( allomorphId, textForm, dialog.glosses() );
            mGlossItem->addAllomorphToAnalysis( allomorph, mWritingSystem );
            MorphologicalAnalysis analysis;
            analysis.addAllomorph( allomorph );
            mDbAdapter->setMorphologicalAnalysis( textForm.id(), analysis );
            createInitializedLayout();
            emit morphologicalAnalysisChanged( mGlossItem->textForm(mWritingSystem).id() );
        }
    }
}

void AnalysisWidget::clearWidgetsFromLayout()
{
    if( mLayout->count() == 0 )
        return;
    QLayoutItem * item;
    while( ( item = mLayout->takeAt(0) ) != 0 )
    {
        delete item->widget();
        delete item;
    }
}
