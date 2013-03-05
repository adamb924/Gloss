#include "dealwithspacesdialog.h"
#include "ui_dealwithspacesdialog.h"

DealWithSpacesDialog::DealWithSpacesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DealWithSpacesDialog)
{
    ui->setupUi(this);

    mResult = DealWithSpacesDialog::CancelEdit;

    connect(ui->splitWord, SIGNAL(clicked()), this, SLOT(splitWord()) );
    connect(ui->convertSpaces, SIGNAL(clicked()), this, SLOT(convertSpaces()) );
    connect(ui->cancelEdit, SIGNAL(clicked()), this, SLOT(cancelEdit()) );
}

DealWithSpacesDialog::~DealWithSpacesDialog()
{
    delete ui;
}

DealWithSpacesDialog::Result DealWithSpacesDialog::choice() const
{
    return mResult;
}

void DealWithSpacesDialog::splitWord()
{
    mResult = DealWithSpacesDialog::SplitWord;
    accept();
}

void DealWithSpacesDialog::convertSpaces()
{
    mResult = DealWithSpacesDialog::ConvertSpaces;
    accept();
}

void DealWithSpacesDialog::cancelEdit()
{
    mResult = DealWithSpacesDialog::CancelEdit;
    accept();
}
