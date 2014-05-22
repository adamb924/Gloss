#include "searchform.h"
#include "ui_searchform.h"

#include "databaseadapter.h"

SearchForm::SearchForm(const DatabaseAdapter *dbAdapter, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchForm)
{
    ui->setupUi(this);

    connect(ui->wsComboBox, SIGNAL(writingSystemSelected(WritingSystem)), ui->searchLineEdit, SLOT(setWritingSystem(WritingSystem)) );
    ui->wsComboBox->setWritingSystems( dbAdapter->writingSystems() );

    ui->interpretationIdLineEdit->setValidator( new QIntValidator(1, 0xffffff, this) );
    ui->textFormLineEdit->setValidator( new QIntValidator(1, 0xffffff, this) );
    ui->glossLineEdit->setValidator( new QIntValidator(1, 0xffffff, this) );
    ui->lexicalEntryLineEdit->setValidator( new QIntValidator(1, 0xffffff, this) );
    ui->allmorphLineEdit->setValidator( new QIntValidator(1, 0xffffff, this) );

    connect( ui->interpretationButton, SIGNAL(clicked()), this, SLOT(interpretationId()) );
    connect( ui->textFormButton, SIGNAL(clicked()), this, SLOT(textFormId()) );
    connect( ui->glossButton, SIGNAL(clicked()), this, SLOT(glossId()) );
    connect( ui->lexicalEntryButton, SIGNAL(clicked()), this, SLOT(lexicalEntryId()) );
    connect( ui->allomorphButton, SIGNAL(clicked()), this, SLOT(allomorphId()) );
    connect( ui->searchLineEdit, SIGNAL(returnPressed()), this, SLOT(text()) );
}

SearchForm::~SearchForm()
{
    delete ui;
}

void SearchForm::setXmlTextWarning(bool relevant)
{
    relevant ? ui->xmlTextWarning->setVisible(true) : ui->xmlTextWarning->setVisible(false);
}

void SearchForm::interpretationId()
{
    emit searchForInterpretationById( ui->interpretationIdLineEdit->text().toLongLong() );
}

void SearchForm::textFormId()
{
    emit searchForTextFormById( ui->textFormLineEdit->text().toLongLong() );
}

void SearchForm::glossId()
{
    emit searchForGlossById( ui->glossLineEdit->text().toLongLong() );
}

void SearchForm::lexicalEntryId()
{
    emit searchForLexicalEntryById( ui->lexicalEntryLineEdit->text().toLongLong() );
}

void SearchForm::allomorphId()
{
    emit searchForAllomorphById( ui->allmorphLineEdit->text().toLongLong() );
}

void SearchForm::text()
{
    if( !ui->searchLineEdit->textBit().text().isEmpty() )
    {
        if( ui->substringSearchCheckbox->isChecked() )
        {
            emit searchForSubstring( ui->searchLineEdit->textBit() );
        }
        else
        {
            emit searchForText( ui->searchLineEdit->textBit() );
        }
    }
}
