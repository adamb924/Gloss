#include "newtextdialog.h"
#include "ui_newtextdialog.h"

#include <QtGui>

NewTextDialog::NewTextDialog(const QList<WritingSystem*> & ws, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    mWritingSystems = ws;

    for(int i=0; i<mWritingSystems.count(); i++)
        ui->baselineWritingSystem->addItem(mWritingSystems.at(i)->summaryString(), mWritingSystems.at(i)->flexString());
}

NewTextDialog::~NewTextDialog()
{
    delete ui;
}

WritingSystem NewTextDialog::writingSystem() const
{
    QString flexString = ui->baselineWritingSystem->itemData(ui->baselineWritingSystem->currentIndex()).toString();
    for(int i=0; i<mWritingSystems.count(); i++)
        if( *mWritingSystems.at(i) == flexString )
            return *mWritingSystems.at(i);
    return WritingSystem();
}

QString NewTextDialog::name() const
{
    return ui->textName->text();
}
