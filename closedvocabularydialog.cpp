#include "closedvocabularydialog.h"
#include "ui_closedvocabularydialog.h"

ClosedVocabularyDialog::ClosedVocabularyDialog(Project * prj, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClosedVocabularyDialog),
    mProject(prj)
{
    ui->setupUi(this);
}

ClosedVocabularyDialog::~ClosedVocabularyDialog()
{
    delete ui;
}
