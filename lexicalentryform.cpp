#include "lexicalentryform.h"
#include "ui_lexicalentryform.h"

#include "databaseadapter.h"
#include "genericlexicalentryform.h"

LexicalEntryForm::LexicalEntryForm(const Allomorph & allomorph, DatabaseAdapter *dbAdapter,  QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LexicalEntryForm)
{
    ui->setupUi(this);
    mAllomorph = allomorph;
    mDbAdapter = dbAdapter;

    setupLayout();
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

void LexicalEntryForm::setupLayout()
{
    // glosses
    QList<WritingSystem> glosses = mDbAdapter->lexicalEntryGlosses();
    foreach(WritingSystem ws, glosses)
    {
        GenericLexicalEntryForm *form = new GenericLexicalEntryForm( ws );
        ui->glossesLayout->addWidget(form);
    }

    // citation forms
    QList<WritingSystem> citationForms = mDbAdapter->lexicalEntryCitationForms();
    foreach(WritingSystem ws, citationForms)
    {
        GenericLexicalEntryForm *form = new GenericLexicalEntryForm( ws );
        ui->glossesLayout->addWidget(form);
    }
}
