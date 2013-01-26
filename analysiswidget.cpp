#include "analysiswidget.h"
#include "createanalysisdialog.h"

#include <QtGui>

#include "glossitem.h"
#include "chooselexicalentriesdialog.h"
#include "createlexicalentrydialog.h"
#include "databaseadapter.h"
#include "immutablelabel.h"

AnalysisWidget::AnalysisWidget(const GlossItem *glossItem, const WritingSystem & analysisWs, const DatabaseAdapter *dbAdapter, QWidget *parent) :
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
    if( mGlossItem->morphologicalAnalysis(mWritingSystem).isEmpty() )
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

void AnalysisWidget::createInitializedLayout(const MorphologicalAnalysis & analysis)
{
    clearWidgetsFromLayout();

    mLayout->addWidget( new ImmutableLabel( TextBit( analysis.baselineSummary() , mWritingSystem ) , false, this ) );

    QList<WritingSystem> glossLines = mDbAdapter->lexicalEntryGlosses();
    for(int i=0; i<glossLines.count(); i++)
        mLayout->addWidget( new ImmutableLabel( TextBit( analysis.glossSummary(glossLines.at(i)), mWritingSystem ), false, this ) );
}

void AnalysisWidget::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);
    menu.addAction(tr("Monomorphemic"), this, SLOT(createMonomorphemicLexicalEntry()));
    menu.addAction(tr("Polymorphemic"), this, SLOT(enterAnalysis()));
    menu.exec(event->globalPos());
}

void AnalysisWidget::enterAnalysis()
{
    TextBit textForm = mGlossItem->textForms()->value(mWritingSystem);
    CreateAnalysisDialog dialog( textForm );
    if( dialog.exec() == QDialog::Accepted )
    {
        ChooseLexicalEntriesDialog leDialog( TextBit(dialog.analysisString(), textForm.writingSystem(), textForm.id() ), mGlossItem,  mDbAdapter , this);
        if( leDialog.exec() == QDialog::Accepted )
        {
            createInitializedLayout( leDialog.morphologicalAnalysis() );
            emit morphologicalAnalysisChanged( leDialog.morphologicalAnalysis() );
        }
    }
}

void AnalysisWidget::createMonomorphemicLexicalEntry()
{
    TextBit textForm = mGlossItem->textForms()->value(mWritingSystem);
    Allomorph allomorph( -1, textForm );
    CreateLexicalEntryDialog dialog( &allomorph, true, mGlossItem, mDbAdapter, this);
    if( dialog.exec() == QDialog::Accepted )
    {
        qlonglong lexicalEntryId = dialog.id();
        if( lexicalEntryId != -1 )
        {
            qlonglong allomorphId = mDbAdapter->addAllomorph( textForm , lexicalEntryId );
            allomorph = mDbAdapter->allomorphFromId(allomorphId);

            MorphologicalAnalysis analysis( mGlossItem->textForms()->value(mWritingSystem) );
            analysis.addAllomorph( allomorph );
            mDbAdapter->setMorphologicalAnalysis( textForm.id(), analysis );

            createInitializedLayout( analysis );
            emit morphologicalAnalysisChanged( analysis );
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
        item->widget()->deleteLater();;
        delete item;
    }
}
