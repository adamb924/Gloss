#include "syntacticanalysisoptionsdialog.h"
#include "ui_syntacticanalysisoptionsdialog.h"

#include "syntacticanalysis.h"

SyntacticAnalysisOptionsDialog::SyntacticAnalysisOptionsDialog(const QList<WritingSystem> &writingSystems, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SyntacticAnalysisOptionsDialog)
{
    ui->setupUi(this);
    ui->writingSystem->setWritingSystems(writingSystems);
}

SyntacticAnalysisOptionsDialog::SyntacticAnalysisOptionsDialog(const SyntacticAnalysis *analysis, const QList<WritingSystem> &writingSystems, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SyntacticAnalysisOptionsDialog)
{
    ui->setupUi(this);
    ui->writingSystem->setWritingSystems(writingSystems);
    ui->name->setText(analysis->name());
    ui->writingSystem->setCurrentWritingSystem(analysis->writingSystem());
    ui->closedVocabulary->setChecked(analysis->closedVocabulary());
}

SyntacticAnalysisOptionsDialog::~SyntacticAnalysisOptionsDialog()
{
    delete ui;
}

WritingSystem SyntacticAnalysisOptionsDialog::writingSystem() const
{
    return ui->writingSystem->currentWritingSystem();
}

QString SyntacticAnalysisOptionsDialog::name() const
{
    return ui->name->text();
}

bool SyntacticAnalysisOptionsDialog::closedVocabulary() const
{
    return ui->closedVocabulary->isChecked();
}
