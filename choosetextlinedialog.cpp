#include "choosetextlinedialog.h"
#include "ui_choosetextlinedialog.h"

ChooseTextLineDialog::ChooseTextLineDialog(const QStringList & textNames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseTextLineDialog)
{
    ui->setupUi(this);

    ui->spinBox->setRange( 1 , 2147483647 );
    ui->comboBox->insertItems(0, textNames);
}

ChooseTextLineDialog::~ChooseTextLineDialog()
{
    delete ui;
}

QString ChooseTextLineDialog::textName() const
{
    return ui->comboBox->currentText();
}

int ChooseTextLineDialog::lineNumber() const
{
    return ui->spinBox->value();
}
