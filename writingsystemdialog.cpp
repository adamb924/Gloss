#include "writingsystemdialog.h"
#include "ui_writingsystemdialog.h"

WritingSystemDialog::WritingSystemDialog(const QList<WritingSystem> & writingSystems, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WritingSystemDialog)
{
    ui->setupUi(this);
    ui->comboBox->setWritingSystems(writingSystems);
}

WritingSystemDialog::~WritingSystemDialog()
{
    delete ui;
}

WritingSystem WritingSystemDialog::writingSystem() const
{
    return ui->comboBox->currentWritingSystem();
}
