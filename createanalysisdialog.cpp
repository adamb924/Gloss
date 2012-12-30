#include "createanalysisdialog.h"
#include "ui_createanalysisdialog.h"

CreateAnalysisDialog::CreateAnalysisDialog(const TextBit & initialString, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateAnalysisDialog)
{
    ui->setupUi(this);
    ui->analysisEdit->setText(initialString.text());
    ui->analysisEdit->setWritingSystem(initialString.writingSystem());

    setWindowTitle(tr("Analyze word"));
}

CreateAnalysisDialog::~CreateAnalysisDialog()
{
    delete ui;
}

QString CreateAnalysisDialog::analysisString() const
{
    return ui->analysisEdit->text();
}
