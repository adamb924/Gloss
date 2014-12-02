#include "createlexicalentrydialog.h"
#include "ui_createlexicalentrydialog.h"

#include <QList>

#include "writingsystem.h"
#include "createlexicalentrydialog.h"
#include "databaseadapter.h"
#include "lingedit.h"
#include "glossitem.h"
#include "textbit.h"
#include "project.h"
#include "lexiconlineform.h"

CreateLexicalEntryDialog::CreateLexicalEntryDialog(qlonglong lexicalEntryId, bool hideGuessButton, const GlossItem *glossItem, const Project *project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateLexicalEntryDialog),
    mProject(project),
    mDbAdapter(mProject->dbAdapter()),
    mGlossItem(glossItem),
    mIsMonomorphemic(false),
    mLexicalEntryId(lexicalEntryId),
    mHideGuessButton(hideGuessButton)
{
    ui->setupUi(this);

    addMorphemeTypes();

    fillFromDatabase();

    ui->grammaticalInformation->setWritingSystem( mProject->metaLanguage() );

    connect(this, SIGNAL(accepted()), this, SLOT(changeLexicalEntry()));

    ui->linkToOther->hide();

    setWindowTitle(tr("Edit lexical entry"));
}

CreateLexicalEntryDialog::CreateLexicalEntryDialog(const TextBit & allomorphString, bool hideGuessButton, bool isMonomorphemic, const GlossItem *glossItem, const Project *project, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CreateLexicalEntryDialog),
        mAllomorphString(allomorphString),
        mProject(project),
        mDbAdapter(mProject->dbAdapter()),
        mGlossItem(glossItem),
        mIsMonomorphemic(isMonomorphemic),
        mLexicalEntryId(-1),
        mHideGuessButton(hideGuessButton)
{
    ui->setupUi(this);

    addMorphemeTypes();

    guessAppropriateValues();

    ui->grammaticalInformation->setWritingSystem( mProject->metaLanguage() );    

    connect(this, SIGNAL(accepted()), this, SLOT(createLexicalEntry()));

    connect( ui->linkToOther, SIGNAL(clicked()), this, SLOT(reject()) );
    connect( ui->linkToOther, SIGNAL(clicked()), this, SIGNAL(linkToOther()) );

    setWindowTitle(tr("Create lexical entry"));
}

CreateLexicalEntryDialog::~CreateLexicalEntryDialog()
{
    delete ui;
}

void CreateLexicalEntryDialog::addMorphemeTypes()
{
    QStringList morphemeTypes;
    morphemeTypes << "Stem" << "Prefix" << "Suffix" << "Infix" << "BoundStem" << "Proclitic" << "Enclitic" << "Simulfix" << "Suprafix";
    ui->morphemeTypeCombo->addItems(morphemeTypes);
}

Allomorph::Type CreateLexicalEntryDialog::morphemeType() const
{
    return Allomorph::getType( ui->morphemeTypeCombo->currentText() );
}

qlonglong CreateLexicalEntryDialog::lexicalEntryId() const
{
    return mLexicalEntryId;
}

void CreateLexicalEntryDialog::guessAppropriateValues()
{
    Allomorph::Type type = Allomorph::typeFromFormattedString( mAllomorphString );
    bool isStem = type == Allomorph::Stem || type == Allomorph::BoundStem;

    ui->morphemeTypeCombo->setCurrentText( Allomorph::getTypeString( type ) );

    QList<WritingSystem> glosses = *( mProject->lexicalEntryGlossFields() );
    foreach( WritingSystem ws , glosses )
    {
        LexiconLineForm *form = new LexiconLineForm( mGlossItem->glosses()->value(ws, TextBit("", ws) ), isStem && mGlossItem->glosses()->contains(ws), mHideGuessButton );
        ui->glossLayout->addWidget(form);
        mGlossEdits << form;
    }

    QList<WritingSystem> citationForms =  *(mProject->lexicalEntryCitationFormFields());
    foreach( WritingSystem ws , citationForms )
    {
        bool autoFill = false;
        TextBit guess = TextBit("", ws);
        if ( ws == mAllomorphString.writingSystem() )
        {
            guess = Allomorph::stripPunctuationFromTextBit(mAllomorphString);
            autoFill = true;
        }
        else if ( mIsMonomorphemic )
        {
            guess = mGlossItem->textForms()->value(ws);
            autoFill = true;
        }
        else
        {
            guess = mGlossItem->textForms()->value(ws, TextBit("", ws) );
            autoFill = false;
        }

        LexiconLineForm *form = new LexiconLineForm( guess, autoFill, mHideGuessButton );
        ui->citationFormLayout->addWidget(form);
        mCitationFormEdits << form;
    }
}

void CreateLexicalEntryDialog::fillFromDatabase()
{
    QList<WritingSystem> glosses = *(mProject->lexicalEntryGlossFields());
    TextBitHash glossValues = mDbAdapter->lexicalEntryGlossFormsForId(mLexicalEntryId);

    foreach( WritingSystem ws , glosses )
    {
        LexiconLineForm *form = new LexiconLineForm( glossValues.value( ws ), true, mHideGuessButton );
        ui->glossLayout->addWidget(form);
        mGlossEdits << form;
    }

    QList<WritingSystem> citationForms = *(mProject->lexicalEntryCitationFormFields());
    TextBitHash citationFormValues = mDbAdapter->lexicalEntryCitationFormsForId(mLexicalEntryId);

    foreach( WritingSystem ws , citationForms )
    {
        LexiconLineForm *form = new LexiconLineForm( citationFormValues.value(ws), true, mHideGuessButton );
        ui->citationFormLayout->addWidget(form);
        mCitationFormEdits << form;
    }

    ui->morphemeTypeCombo->setCurrentText( Allomorph::getTypeString( mDbAdapter->lexicalEntryMorphologicalType(mLexicalEntryId) ) );

    QStringList tags = mDbAdapter->grammaticalTags(mLexicalEntryId);
    ui->grammaticalInformation->setText( tags.join(' ') );
}

void CreateLexicalEntryDialog::createLexicalEntry()
{
    QList<TextBit> glosses;
    QList<TextBit> citationForms;

    for(int i=0; i<mGlossEdits.count(); i++)
        glosses << mGlossEdits.at(i)->textBit();

    for(int i=0; i<mCitationFormEdits.count(); i++)
        citationForms << mCitationFormEdits.at(i)->textBit();

    mLexicalEntryId = mDbAdapter->addLexicalEntry( ui->grammaticalInformation->text(), morphemeType(), glosses, citationForms, grammaticalTags() );
}

void CreateLexicalEntryDialog::changeLexicalEntry()
{
    for(int i=0; i<mGlossEdits.count(); i++)
        mDbAdapter->updateLexicalEntryGloss( mLexicalEntryId , mGlossEdits.at(i)->textBit() );

    for(int i=0; i<mCitationFormEdits.count(); i++)
        mDbAdapter->updateLexicalEntryCitationForm( mLexicalEntryId , mCitationFormEdits.at(i)->textBit() );

    mDbAdapter->setLexicalEntryMorphologicalType( mLexicalEntryId, morphemeType() );

    mDbAdapter->setTagsForLexicalEntry( mLexicalEntryId , grammaticalTags() );
}

TextBitHash CreateLexicalEntryDialog::glosses() const
{
    TextBitHash glosses;
    for(int i=0; i<mGlossEdits.count(); i++)
        glosses.insert( mGlossEdits.at(i)->textBit().writingSystem(),  mGlossEdits.at(i)->textBit() );
    return glosses;
}

QStringList CreateLexicalEntryDialog::grammaticalTags() const
{
    return ui->grammaticalInformation->text().split(QRegExp("\\s+"), QString::SkipEmptyParts);
}
