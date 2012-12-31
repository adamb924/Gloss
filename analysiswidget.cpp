#include "analysiswidget.h"
#include "createanalysisdialog.h"

#include <QtGui>

#include "glossitem.h"
#include "chooselexicalentriesdialog.h"
#include "createlexicalentrydialog.h"
#include "databaseadapter.h"

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
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    QPushButton *analyze = new QPushButton(tr("Analyze"), this);
    analyze->setFlat(true);
    analyze->setStyleSheet("QPushButton { color: blue; text-decoration: underline; padding: 0px; }");
    layout->addWidget(analyze);
    connect(analyze, SIGNAL(clicked()), this, SLOT(enterAnalysis()));

    QPushButton *createMle = new QPushButton(tr("Create MLE"), this);
    createMle->setToolTip(tr("Create monomorphemic lexical entry"));
    createMle->setFlat(true);
    createMle->setStyleSheet("QPushButton { color: blue; text-decoration: underline; padding: 0px; }");
    layout->addWidget(createMle);
    connect(createMle, SIGNAL(clicked()), this, SLOT(createMonomorphemicLexicalEntry()));
}

void AnalysisWidget::enterAnalysis()
{
    CreateAnalysisDialog dialog( mGlossItem->textForm( mWritingSystem ) );
    if( dialog.exec() == QDialog::Accepted )
    {
        ChooseLexicalEntriesDialog leDialog( TextBit(dialog.analysisString(), mWritingSystem), mGlossItem,  mDbAdapter , this);
        leDialog.exec();
    }
}

void AnalysisWidget::createMonomorphemicLexicalEntry()
{
    CreateLexicalEntryDialog dialog(mGlossItem->textForm(mWritingSystem), mGlossItem, mDbAdapter, this);
    if( dialog.exec() == QDialog::Accepted )
    {
        qlonglong id = dialog.id();
        if( id != -1 )
            mDbAdapter->addAllomorph( mGlossItem->textForm(mWritingSystem) , id );
    }
}
