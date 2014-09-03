#include "createsyntacticanalysisdialog.h"
#include "ui_createsyntacticanalysisdialog.h"

CreateSyntacticAnalysisDialog::CreateSyntacticAnalysisDialog(const QList<WritingSystem> &writingSystems, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateSyntacticAnalysisDialog)
{
    ui->setupUi(this);
    ui->writingSystem->setWritingSystems(writingSystems);
}

CreateSyntacticAnalysisDialog::~CreateSyntacticAnalysisDialog()
{
    delete ui;
}

WritingSystem CreateSyntacticAnalysisDialog::writingSystem() const
{
    return ui->writingSystem->currentWritingSystem();
}

QString CreateSyntacticAnalysisDialog::name() const
{
    return ui->name->text();
}
