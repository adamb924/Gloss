#include "searchreplaceform.h"
#include "ui_searchreplaceform.h"

#include "databaseadapter.h"

SearchReplaceForm::SearchReplaceForm(const QString & checkboxLabel, const InterlinearItemType & type, DatabaseAdapter *dbAdapter, QWidget *parent) :
        QWidget(parent),
        ui(new Ui::SearchReplaceForm)
{
    mDbAdapter = dbAdapter;
    mType = type;
    ui->setupUi(this);

    ui->checkBox->setText( checkboxLabel );
    ui->label->setText("");
    ui->lineEdit->setValidator( new QIntValidator(1, 0xffffff , ui->lineEdit ) );

    connect(ui->checkBox, SIGNAL(toggled(bool)), ui->lineEdit, SLOT(setEnabled(bool)) );
    connect(ui->checkBox, SIGNAL(toggled(bool)), this, SIGNAL(checkboxChanged(bool)) );
    if( mType.type() != InterlinearItemType::Null )
        connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(setFromDatabase(QString)) );
}

SearchReplaceForm::~SearchReplaceForm()
{
    delete ui;
}

void SearchReplaceForm::setFromDatabase( const QString & databaseIndex )
{
    qlonglong id = databaseIndex.toLongLong();
    TextBit bit;
    if( mType.type() == InterlinearItemType::Text || mType.type() == InterlinearItemType::ImmutableText )
        bit = mDbAdapter->textFormFromId( id );
    else if ( mType.type() == InterlinearItemType::Gloss || mType.type() == InterlinearItemType::ImmutableGloss)
        bit = mDbAdapter->glossFromId( id );
    ui->label->setText( bit.text() );
    if( mType.writingSystem() == bit.writingSystem() )
        ui->label->setStyleSheet("QLabel { color: black; }");
    else
        ui->label->setStyleSheet("QLabel { color: red; }");
}

InterlinearItemType SearchReplaceForm::type() const
{
    return mType;
}

qlonglong SearchReplaceForm::id() const
{
    if( ui->checkBox->isChecked() && !ui->lineEdit->text().isEmpty() )
        return ui->lineEdit->text().toLongLong();
    else
        return -1;
}

void SearchReplaceForm::setCheckboxChecked(bool enabled)
{
    ui->checkBox->setChecked(enabled);
}
