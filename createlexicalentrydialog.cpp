#include "createlexicalentrydialog.h"
#include "ui_createlexicalentrydialog.h"

#include <QList>

#include "writingsystem.h"
#include "createlexicalentrydialog.h"
#include "databaseadapter.h"
#include "lingedit.h"
#include "glossitem.h"
#include "textbit.h"

CreateLexicalEntryDialog::CreateLexicalEntryDialog(const Allomorph * allomorph, bool isMonomorphemic, const GlossItem *glossItem, const DatabaseAdapter *dbAdapter, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CreateLexicalEntryDialog)
{
    mDbAdapter = dbAdapter;
    mAllomorph = allomorph;
    mGlossItem = glossItem;
    mIsMonomorphemic = isMonomorphemic;

    mId = -1;

    ui->setupUi(this);
    fillData();

    ui->grammaticalInformation->setWritingSystem( dbAdapter->metaLanguage() );

    connect(this, SIGNAL(accepted()), this, SLOT(createLexicalEntry()));

    connect( ui->linkToOther, SIGNAL(clicked()), this, SLOT(reject()) );
    connect( ui->linkToOther, SIGNAL(clicked()), this, SIGNAL(linkToOther()) );

    setWindowTitle(tr("Create lexical entry"));
}

CreateLexicalEntryDialog::~CreateLexicalEntryDialog()
{
    delete ui;
}

qlonglong CreateLexicalEntryDialog::id() const
{
    return mId;
}

void CreateLexicalEntryDialog::fillData()
{
    QList<WritingSystem> glosses = mDbAdapter->lexicalEntryGlosses();
    foreach( WritingSystem ws , glosses )
    {
        LingEdit *edit = new LingEdit( TextBit("", ws) );
        ui->glossLayout->addWidget(edit);
        mGlossEdits << edit;
        if( mAllomorph->isStem() && mGlossItem->glosses()->contains(ws) )
            edit->setText( mGlossItem->glosses()->value(ws).text() );
    }

    QList<WritingSystem> citationForms = mDbAdapter->lexicalEntryCitationForms();
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

    // TODO read the grammatical tags from the database
}

void CreateLexicalEntryDialog::createLexicalEntry()
{
    QList<TextBit> glosses;
    QList<TextBit> citationForms;

    for(int i=0; i<mGlossEdits.count(); i++)
        glosses << mGlossEdits.at(i)->textBit();

    for(int i=0; i<mCitationFormEdits.count(); i++)
        citationForms << mCitationFormEdits.at(i)->textBit();

    mId = mDbAdapter->addLexicalEntry( ui->grammaticalInformation->text(), mAllomorph->type(), glosses, citationForms, grammaticalTags() );
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
