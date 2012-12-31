#include "lexicalentryform.h"
#include "ui_lexicalentryform.h"

#include "databaseadapter.h"
#include "genericlexicalentryform.h"
#include "createlexicalentrydialog.h"

LexicalEntryForm::LexicalEntryForm(const Allomorph & allomorph, GlossItem *glossItem, DatabaseAdapter *dbAdapter,  QWidget *parent) :
        QWidget(parent),
        ui(new Ui::LexicalEntryForm)
{
    ui->setupUi(this);
    mAllomorph = allomorph;
    mDbAdapter = dbAdapter;
    mGlossItem = glossItem;

    fillData();

    connect(ui->newForm, SIGNAL(clicked()), this, SLOT(newLexicalEntry()));
}

LexicalEntryForm::~LexicalEntryForm()
{
    delete ui;
}

void LexicalEntryForm::fillData()
{
    ui->formLabel->setText( mAllomorph.text() );
    ui->morphemeTypeLabel->setText( mAllomorph.typeString() );

    ui->candidatesCombo->clear();
    QHash<qlonglong,QString> candidates = mDbAdapter->getLexicalEntryCandidates( mAllomorph.textBit());
    QHashIterator<qlonglong,QString> iter(candidates);

    while(iter.hasNext())
    {
        iter.next();
        ui->candidatesCombo->addItem( iter.value(), iter.key() );
    }
    ui->candidatesCombo->setEnabled( ui->candidatesCombo->count() != 0 );
}

void LexicalEntryForm::newLexicalEntry()
{
    CreateLexicalEntryDialog dialog(mAllomorph.textBit(), mGlossItem, mDbAdapter, this);
    if( dialog.exec() == QDialog::Accepted )
    {
        if( dialog.id() != -1 )
        {
            mDbAdapter->addAllomorph( mAllomorph.textBit() , dialog.id() );
            fillData();
        }
    }
}
