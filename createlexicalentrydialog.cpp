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
    mAllomorph(0),
    mIsMonomorphemic(false),
    mLexicalEntryId(lexicalEntryId),
    mHideGuessButton(hideGuessButton)
{
    ui->setupUi(this);

    fillFromDatabase();

    ui->grammaticalInformation->setWritingSystem( mProject->metaLanguage() );

    connect(this, SIGNAL(accepted()), this, SLOT(changeLexicalEntry()));

    ui->linkToOther->hide();

    setWindowTitle(tr("Edit lexical entry"));
}

CreateLexicalEntryDialog::CreateLexicalEntryDialog(const Allomorph * allomorph, bool hideGuessButton, bool isMonomorphemic, const GlossItem *glossItem, const Project *project, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CreateLexicalEntryDialog),
        mHideGuessButton(hideGuessButton),
        mProject(project),
        mDbAdapter(mProject->dbAdapter()),
        mGlossItem(glossItem),
        mAllomorph(allomorph),
        mIsMonomorphemic(isMonomorphemic),
        mLexicalEntryId(-1)
{
    ui->setupUi(this);

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

qlonglong CreateLexicalEntryDialog::lexicalEntryId() const
{
    return mLexicalEntryId;
}

void CreateLexicalEntryDialog::guessAppropriateValues()
{
    QList<WritingSystem> glosses = *( mProject->lexicalEntryGlossFields() );
    foreach( WritingSystem ws , glosses )
    {
        LexiconLineForm *form = new LexiconLineForm( mGlossItem->glosses()->value(ws, TextBit("", ws) ), mAllomorph->isStem() && mGlossItem->glosses()->contains(ws), mHideGuessButton );
        ui->glossLayout->addWidget(form);
        mGlossEdits << form;
    }

    QList<WritingSystem> citationForms =  *(mProject->lexicalEntryCitationFormFields());
    foreach( WritingSystem ws , citationForms )
    {
        bool autoFill = false;
        TextBit guess = TextBit("", ws);
        if ( ws == mAllomorph->writingSystem() )
        {
            guess = mAllomorph->textBit();
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

    mLexicalEntryId = mDbAdapter->addLexicalEntry( ui->grammaticalInformation->text(), mAllomorph->type(), glosses, citationForms, grammaticalTags() );
}

void CreateLexicalEntryDialog::changeLexicalEntry()
{
    for(int i=0; i<mGlossEdits.count(); i++)
        mDbAdapter->updateLexicalEntryGloss( mLexicalEntryId , mGlossEdits.at(i)->textBit() );

    for(int i=0; i<mCitationFormEdits.count(); i++)
        mDbAdapter->updateLexicalEntryCitationForm( mLexicalEntryId , mCitationFormEdits.at(i)->textBit() );

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
