#include "createlexicalentrydialog.h"
#include "ui_createlexicalentrydialog.h"

#include <QList>

#include "writingsystem.h"
#include "createlexicalentrydialog.h"
#include "databaseadapter.h"
#include "lingedit.h"
#include "glossitem.h"
#include "textbit.h"

CreateLexicalEntryDialog::CreateLexicalEntryDialog(const Allomorph & allomorph, GlossItem *glossItem, DatabaseAdapter *dbAdapter, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CreateLexicalEntryDialog)
{
    mDbAdapter = dbAdapter;
    mAllomorph = allomorph;
    mGlossItem = glossItem;

    mId = -1;

    ui->setupUi(this);
    fillData();

    connect(this, SIGNAL(accepted()), this, SLOT(createLexicalEntry()));

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
        if( mGlossItem->glosses()->contains(ws) )
            edit->setText( mGlossItem->glosses()->value(ws).text() );
    }

    QList<WritingSystem> citationForms = mDbAdapter->lexicalEntryCitationForms();
    foreach( WritingSystem ws , citationForms )
    {
        LingEdit *edit = new LingEdit( TextBit("", ws) );
        ui->citationFormLayout->addWidget(edit);
        mCitationFormEdits << edit;
        if( mGlossItem->textForms()->contains(ws) )
            edit->setText( mGlossItem->textForms()->value(ws).text() );
    }
}

void CreateLexicalEntryDialog::createLexicalEntry()
{
    QList<TextBit> glosses;
    QList<TextBit> citationForms;

    for(int i=0; i<mGlossEdits.count(); i++)
        glosses << mGlossEdits.at(i)->textBit();

    for(int i=0; i<mCitationFormEdits.count(); i++)
        citationForms << mCitationFormEdits.at(i)->textBit();

    mId = mDbAdapter->addLexicalEntry( ui->grammaticalInformation->text(), glosses, citationForms );
}
