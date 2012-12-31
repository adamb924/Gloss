#include "createlexicalentrydialog.h"
#include "ui_createlexicalentrydialog.h"

CreateLexicalEntryDialog::CreateLexicalEntryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateLexicalEntryDialog)
{
    ui->setupUi(this);
}

CreateLexicalEntryDialog::~CreateLexicalEntryDialog()
{
    delete ui;
}
