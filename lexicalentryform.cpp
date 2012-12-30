#include "lexicalentryform.h"
#include "ui_lexicalentryform.h"

LexicalEntryForm::LexicalEntryForm(const Allomorph & allomorph, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LexicalEntryForm)
{
    ui->setupUi(this);
    mAllomorph = allomorph;
    fillData();
}

LexicalEntryForm::~LexicalEntryForm()
{
    delete ui;
}

void LexicalEntryForm::fillData()
{
    ui->formLabel->setText( mAllomorph.text() );
    ui->morphemeTypeLabel->setText( mAllomorph.typeString() );
}
