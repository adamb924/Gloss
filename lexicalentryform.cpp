#include "lexicalentryform.h"
#include "ui_lexicalentryform.h"

#include "databaseadapter.h"
#include "createlexicalentrydialog.h"
#include "lexicalentrysearchdialog.h"

LexicalEntryForm::LexicalEntryForm(const Allomorph & allomorph, const GlossItem *glossItem, const DatabaseAdapter *dbAdapter,  QWidget *parent) :
        QWidget(parent),
        ui(new Ui::LexicalEntryForm)
{
    ui->setupUi(this);
    mAllomorph = allomorph;
    mDbAdapter = dbAdapter;
    mGlossItem = glossItem;

    // the available types are whatever is available in the database, plus whatever the user entered
    mTypes = mDbAdapter->getPossibleMorphologicalTypes( mAllomorph.textBit() );

    mTypes << mAllomorph.type();

    fillData();

    connect(ui->candidatesCombo, SIGNAL(currentIndexChanged(int)), this, SIGNAL(entryChanged()) );
    connect(ui->newForm, SIGNAL(clicked()), this, SLOT(newLexicalEntry()));
    connect(ui->linkToOther, SIGNAL(clicked()), this, SLOT(linkToOther()));
}

LexicalEntryForm::~LexicalEntryForm()
{
    delete ui;
}

void LexicalEntryForm::fillData(qlonglong currentLexicalEntryId)
{
    ui->formLabel->setText( mAllomorph.text() );

    fillTypes();

    ui->candidatesCombo->clear();

    int currentIndex = 0;

    QHash<qlonglong,QString> candidates = mDbAdapter->getLexicalEntryCandidates( mAllomorph.textBit(), mAllomorph.typeString() );
    QHashIterator<qlonglong,QString> iter(candidates);

    while(iter.hasNext())
    {
        iter.next();
        if( currentLexicalEntryId == iter.key() )
            currentIndex = ui->candidatesCombo->count();
        ui->candidatesCombo->addItem( iter.value(), iter.key() );
    }
    ui->candidatesCombo->setEnabled( ui->candidatesCombo->count() != 0 );
    ui->candidatesCombo->setCurrentIndex(currentIndex);
}

void LexicalEntryForm::fillTypes()
{
    disconnect(ui->morphemeType, SIGNAL(currentIndexChanged(int)), this, SLOT(setType(int)));
    ui->morphemeType->clear();
    int currentIndex=0;
    for(int i=0; i<10; i++)
    {
        if( mTypes.contains( (Allomorph::Type)i ))
        {
            ui->morphemeType->addItem( Allomorph::getTypeString( (Allomorph::Type)i ) , i );
            if( mAllomorph.type() == (Allomorph::Type)i )
                currentIndex = ui->morphemeType->count()-1;
        }
    }
    ui->morphemeType->setCurrentIndex(currentIndex);
    connect(ui->morphemeType, SIGNAL(currentIndexChanged(int)), this, SLOT(setType(int)));
}

void LexicalEntryForm::newLexicalEntry()
{
    CreateLexicalEntryDialog dialog(&mAllomorph, false, mGlossItem, mDbAdapter, this);
    connect( &dialog, SIGNAL(linkToOther()), this, SLOT(linkToOther()) );
    if( dialog.exec() == QDialog::Accepted )
    {
        qlonglong lexicalEntryId = dialog.lexicalEntryId();
        if( lexicalEntryId != -1 )
        {
            mDbAdapter->addAllomorph( mAllomorph.textBit() , lexicalEntryId );
            fillData( lexicalEntryId );
            emit entryChanged();
        }
    }
}

void LexicalEntryForm::linkToOther()
{
    LexicalEntrySearchDialog dialog(mDbAdapter, this);
    if( dialog.exec() == QDialog::Accepted )
    {
        qlonglong lexicalEntryId = dialog.lexicalEntryId();
        if( lexicalEntryId != -1 )
        {
            mDbAdapter->addAllomorph( mAllomorph.textBit() , lexicalEntryId );
            fillData(lexicalEntryId);
            emit entryChanged();
        }
    }
}

qlonglong LexicalEntryForm::id() const
{
    if( ui->candidatesCombo->currentIndex() == -1)
        return -1;
    else
        return ui->candidatesCombo->itemData( ui->candidatesCombo->currentIndex() ).toLongLong();
}

TextBit LexicalEntryForm::textBit() const
{
    return mAllomorph.textBit();
}

void LexicalEntryForm::setType(int index)
{
    if( index > -1 && mAllomorph.type() != (Allomorph::Type)ui->morphemeType->itemData(index).toInt() )
    {
        mAllomorph.setType(  (Allomorph::Type)ui->morphemeType->itemData(index).toInt() );
        fillData();
    }
}
