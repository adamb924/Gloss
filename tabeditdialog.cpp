#include "tabeditdialog.h"
#include "ui_tabeditdialog.h"

TabEditDialog::TabEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TabEditDialog)
{
    ui->setupUi(this);
}

TabEditDialog::~TabEditDialog()
{
    delete ui;
}

Tab::TabType TabEditDialog::type() const
{
    return ui->tabTypeCombo->currentIndex() == 0 ? Tab::InterlinearDisplay : Tab::SyntacticParsing;
}

QString TabEditDialog::name() const
{
    return ui->nameLineEdit->text();
}
