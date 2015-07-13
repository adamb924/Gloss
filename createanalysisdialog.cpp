#include "createanalysisdialog.h"
#include "ui_createanalysisdialog.h"

#include "databaseadapter.h"
#include "segmentationoptionbutton.h"

#include <QtDebug>

CreateAnalysisDialog::CreateAnalysisDialog(const DatabaseAdapter *dbAdapter, const TextBit & initialString, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateAnalysisDialog),
    mDbAdapter(dbAdapter)
{
    ui->setupUi(this);
    ui->analysisEdit->setText(initialString.text());
    ui->analysisEdit->setWritingSystem(initialString.writingSystem());

    ui->analysisEdit->setFontSize(24);

    QSet<TextBit> options = mDbAdapter->candidateMorphologicalSplits( initialString );
    foreach(TextBit option, options)
    {
        SegmentationOptionButton * button = new SegmentationOptionButton(option);
        ui->presetOptions->addWidget(button);
        connect(button, SIGNAL(clicked(TextBit)), this, SLOT(acceptSegmentation(TextBit)) );
        connect(button, SIGNAL(rightClicked(TextBit)), this, SLOT(copySegmentation(TextBit)) );
    }

    setWindowTitle(tr("Analyze word"));
}

CreateAnalysisDialog::~CreateAnalysisDialog()
{
    delete ui;
}

QString CreateAnalysisDialog::analysisString() const
{
    if( mSegmentation.text().isEmpty() )
        return ui->analysisEdit->text();
    else
        return mSegmentation.text();
}

void CreateAnalysisDialog::acceptSegmentation(const TextBit &textBit)
{
    mSegmentation = textBit;
    accept();
}

void CreateAnalysisDialog::copySegmentation(const TextBit &textBit)
{
    ui->analysisEdit->setTextBit(textBit);
}
