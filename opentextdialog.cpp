#include "opentextdialog.h"
#include "ui_opentextdialog.h"

OpenTextDialog::OpenTextDialog(const QStringList &textNames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenTextDialog)
{
    ui->setupUi(this);
    ui->textCombo->addItems(textNames);
}

OpenTextDialog::~OpenTextDialog()
{
    delete ui;
}

QString OpenTextDialog::textName() const
{
    return ui->textCombo->currentText();
}

int OpenTextDialog::linesPerScreen() const
{
    return ui->loadEntireText->isChecked() ? -1 : ui->numberOfLinesSpinner->value();
}

int OpenTextDialog::goToLine() const
{
    return ui->goToLineSpinner->value();
}
