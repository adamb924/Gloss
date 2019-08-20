#include "lexicalentryform.h"
#include "ui_lexicalentryform.h"

#include "databaseadapter.h"
#include "createlexicalentrydialog.h"
#include "lexicalentrysearchdialog.h"
#include "project.h"

LexicalEntryForm::LexicalEntryForm(const TextBit &allomorphString, const GlossItem *glossItem, const Project *project,  QWidget *parent) :
        QWidget(parent), ui(new Ui::LexicalEntryForm),
        mProject(project),
        mDbAdapter(mProject->dbAdapter()),
        mGlossItem(glossItem)
{
    ui->setupUi(this);

    mAllomorphText = Allomorph::stripPunctuationFromTextBit(allomorphString);
    mAllomorphType = Allomorph::typeFromFormattedString(allomorphString);

    // the available types are whatever is available in the database, plus whatever the user entered
    mTypes = mDbAdapter->getPossibleMorphologicalTypes( mAllomorphText );

    mTypes << mAllomorphType;

    fillData();

    connect(ui->candidatesCombo, SIGNAL(currentIndexChanged(int)), this, SIGNAL(entryChanged()) );
    connect(ui->newForm, SIGNAL(clicked()), this, SLOT(newLexicalEntry()));
    connect(ui->linkToOther, SIGNAL(clicked()), this, SLOT(linkToOther()));
    connect(ui->quickLeButton, SIGNAL(clicked()), this, SLOT(quickLE()) );
}

LexicalEntryForm::~LexicalEntryForm()
{
    delete ui;
}

void LexicalEntryForm::fillData(qlonglong currentLexicalEntryId)
{
    ui->formLabel->setText( mAllomorphText.text() );

    fillTypes();

    ui->candidatesCombo->clear();

    int currentIndex = 0;

    QList< QPair<qlonglong,QString> > candidates = mDbAdapter->getLexicalEntryCandidates( mAllomorphText, Allomorph::getTypeString(mAllomorphType) );
    QListIterator< QPair<qlonglong,QString> > iter(candidates);

    while(iter.hasNext())
    {
        QPair<qlonglong,QString> pair = iter.next();
        if( currentLexicalEntryId == pair.first )
            currentIndex = ui->candidatesCombo->count();
        ui->candidatesCombo->addItem( pair.second, pair.first );
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
        if( mTypes.contains( static_cast<Allomorph::Type>(i) ))
        {
            ui->morphemeType->addItem( Allomorph::getTypeString( static_cast<Allomorph::Type>(i) ) , i );
            if( mAllomorphType == static_cast<Allomorph::Type>(i) )
                currentIndex = ui->morphemeType->count()-1;
        }
    }
    ui->morphemeType->setCurrentIndex(currentIndex);
    connect(ui->morphemeType, SIGNAL(currentIndexChanged(int)), this, SLOT(setType(int)));
}

void LexicalEntryForm::newLexicalEntry()
{
    CreateLexicalEntryDialog dialog( Allomorph::getTypeFormatTextBit( mAllomorphText, mAllomorphType ), false, false, mGlossItem, mProject, this);
    connect( &dialog, SIGNAL(linkToOther()), this, SLOT(linkToOther()) );
    if( dialog.exec() == QDialog::Accepted )
    {
        qlonglong lexicalEntryId = dialog.lexicalEntryId();
        if( lexicalEntryId != -1 )
        {
            mDbAdapter->addAllomorph( mAllomorphText , lexicalEntryId );
            fillData( lexicalEntryId );
            emit entryChanged();
        }
    }
    ui->newForm->clearFocus();
}

void LexicalEntryForm::linkToOther()
{
    LexicalEntrySearchDialog dialog(mProject, this);
    if( dialog.exec() == QDialog::Accepted )
    {
        qlonglong lexicalEntryId = dialog.lexicalEntryId();
        if( lexicalEntryId != -1 )
        {
            Allomorph::Type type = mDbAdapter->lexicalEntryMorphologicalType( lexicalEntryId );
            mTypes << type;
            mDbAdapter->addAllomorph( mAllomorphText , lexicalEntryId );
            fillData(lexicalEntryId);
            emit entryChanged();
        }
    }
}

void LexicalEntryForm::quickLE()
{
    TextBitHash textForms = * mGlossItem->textForms();
    textForms.insert( mAllomorphText.writingSystem(), mAllomorphText );
    qlonglong lexicalEntryId = mDbAdapter->addLexicalEntry( "", mAllomorphType, mGlossItem->glosses()->values(), textForms.values(), QStringList() );
    Allomorph::Type type = mDbAdapter->lexicalEntryMorphologicalType( lexicalEntryId );
    mTypes << type;
    mDbAdapter->addAllomorph( mAllomorphText , lexicalEntryId );
    fillData(lexicalEntryId);
    emit entryChanged();
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
    return mAllomorphText;
}

Allomorph::Type LexicalEntryForm::type() const
{
    return mAllomorphType;
}

void LexicalEntryForm::setType(int index)
{
    if( index > -1 && mAllomorphType != static_cast<Allomorph::Type>(ui->morphemeType->itemData(index).toInt()) )
    {
        mAllomorphType = static_cast<Allomorph::Type>(ui->morphemeType->itemData(index).toInt());
        fillData();
    }
}
