#include "newtextdialog.h"
#include "ui_newtextdialog.h"

#include <QtGui>

NewTextDialog::NewTextDialog(const QList<WritingSystem*> & ws, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    for(int i=0; i<ws.count(); i++)
        ui->baselineWritingSystem->addItem(ws.at(i)->summaryString(), ws.at(i)->flexString());
}

NewTextDialog::~NewTextDialog()
{
    delete ui;
}

QString NewTextDialog::writingSystem() const
{
    return ui->baselineWritingSystem->itemData(ui->baselineWritingSystem->currentIndex()).toString();
}

QString NewTextDialog::name() const
{
    return ui->textName->text();
}
