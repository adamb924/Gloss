#include "analysiswidget.h"
#include "createanalysisdialog.h"

#include <QtGui>

#include "glossitem.h"
#include "createlexicalentriesdialog.h"

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

    QPushButton *analyze = new QPushButton(tr("Analyze word"), this);
    analyze->setFlat(true);
    analyze->setStyleSheet("QPushButton { color: blue; text-decoration: underline; padding: 0px; }");
    layout->addWidget(analyze);
    connect(analyze, SIGNAL(clicked()), this, SLOT(enterAnalysis()));
}

void AnalysisWidget::enterAnalysis()
{
    CreateAnalysisDialog dialog( mGlossItem->textForm( mWritingSystem ) );
    if( dialog.exec() == QDialog::Accepted )
    {
        qDebug() << dialog.analysisString();
        CreateLexicalEntriesDialog leDialog( TextBit( dialog.analysisString(), mWritingSystem), mDbAdapter , this);
        leDialog.exec();
    }
}
