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

CreateLexicalEntryDialog::CreateLexicalEntryDialog(qlonglong lexicalEntryId, const GlossItem *glossItem, const Project *project, QWidget *parent) :
    mProject(project), QDialog(parent),
    ui(new Ui::CreateLexicalEntryDialog)
{
    mDbAdapter = mProject->dbAdapter();
    mAllomorph = 0;
    mGlossItem = glossItem;
    mIsMonomorphemic = false;
    mLexicalEntryId = lexicalEntryId;

    ui->setupUi(this);
    fillData();

    ui->grammaticalInformation->setWritingSystem( mProject->metaLanguage() );

    connect(this, SIGNAL(accepted()), this, SLOT(changeLexicalEntry()));

    ui->linkToOther->hide();

    setWindowTitle(tr("Edit lexical entry"));
}

CreateLexicalEntryDialog::CreateLexicalEntryDialog(const Allomorph * allomorph, bool isMonomorphemic, const GlossItem *glossItem, const Project *project, QWidget *parent) :
        mProject(project), QDialog(parent),
        ui(new Ui::CreateLexicalEntryDialog)
{
    mDbAdapter = mProject->dbAdapter();
    mAllomorph = allomorph;
    mGlossItem = glossItem;
    mIsMonomorphemic = isMonomorphemic;

    mLexicalEntryId = -1;

    ui->setupUi(this);
    fillData();

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

void CreateLexicalEntryDialog::fillData()
{
    if( mLexicalEntryId == -1 )
        guessAppropriateValues();
    else
        fillFromDatabase();
}

void CreateLexicalEntryDialog::guessAppropriateValues()
{
    QList<WritingSystem> glosses = *( mProject->lexicalEntryGlossFields() );
    foreach( WritingSystem ws , glosses )
    {
        LingEdit *edit = new LingEdit( TextBit("", ws) );
        ui->glossLayout->addWidget(edit);
        mGlossEdits << edit;
        if( mAllomorph->isStem() && mGlossItem->glosses()->contains(ws) )
            edit->setText( mGlossItem->glosses()->value(ws).text() );
    }

    QList<WritingSystem> citationForms =  *(mProject->lexicalEntryCitationFormFields());
    foreach( WritingSystem ws , citationForms )
    {
        LingEdit *edit = new LingEdit( TextBit("", ws) );
        ui->citationFormLayout->addWidget(edit);
        mCitationFormEdits << edit;

        if ( ws == mAllomorph->writingSystem()  )
            edit->setText( mAllomorph->text() );
        else if ( mIsMonomorphemic )
            edit->setText( mGlossItem->textForms()->value(ws).text() );
    }
}

void CreateLexicalEntryDialog::fillFromDatabase()
{
    QList<WritingSystem> glosses = *(mProject->lexicalEntryGlossFields());
    TextBitHash glossValues = mDbAdapter->lexicalEntryGlossFormsForId(mLexicalEntryId);

    foreach( WritingSystem ws , glosses )
    {
        LingEdit *edit = new LingEdit( TextBit("", ws) );
        ui->glossLayout->addWidget(edit);
        mGlossEdits << edit;
        edit->setText( glossValues.value( ws ).text() );
    }

    QList<WritingSystem> citationForms = *(mProject->lexicalEntryCitationFormFields());
    TextBitHash citationFormValues = mDbAdapter->lexicalEntryCitationFormsForId(mLexicalEntryId);

    foreach( WritingSystem ws , citationForms )
    {
        LingEdit *edit = new LingEdit( TextBit("", ws) );
        ui->citationFormLayout->addWidget(edit);
        mCitationFormEdits << edit;

        edit->setText( citationFormValues.value(ws).text() );
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
