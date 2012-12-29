#include "xqueryinputdialog.h"
#include "ui_xqueryinputdialog.h"

XQueryInputDialog::XQueryInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XQueryInputDialog)
{
    ui->setupUi(this);
}

XQueryInputDialog::~XQueryInputDialog()
{
    delete ui;
}

QString XQueryInputDialog::query() const
{
    return ui->queryString->toPlainText();
}
