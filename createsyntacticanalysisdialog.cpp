#include "createsyntacticanalysisdialog.h"
#include "ui_createsyntacticanalysisdialog.h"

#include "syntacticanalysis.h"

CreateSyntacticAnalysisDialog::CreateSyntacticAnalysisDialog(const QList<WritingSystem> &writingSystems, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateSyntacticAnalysisDialog)
{
    ui->setupUi(this);
    ui->writingSystem->setWritingSystems(writingSystems);
}

CreateSyntacticAnalysisDialog::CreateSyntacticAnalysisDialog(const SyntacticAnalysis *analysis, const QList<WritingSystem> &writingSystems, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateSyntacticAnalysisDialog)
{
    ui->setupUi(this);
    ui->writingSystem->setWritingSystems(writingSystems);
    ui->name->setText(analysis->name());
    ui->writingSystem->setCurrentWritingSystem(analysis->writingSystem());
    ui->closedVocabulary->setChecked(analysis->closedVocabulary());
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

bool CreateSyntacticAnalysisDialog::closedVocabulary() const
{
    return ui->closedVocabulary->isChecked();
}
