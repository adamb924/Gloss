#include "chooselexicalentriesdialog.h"

#include "lexicalentryform.h"
#include "morphologicalanalysis.h"
#include "allomorph.h"
#include "databaseadapter.h"
#include "glossitem.h"

#include <QtWidgets>
#include <QtDebug>

ChooseLexicalEntriesDialog::ChooseLexicalEntriesDialog(const TextBit & parseString, const GlossItem *glossItem, const DatabaseAdapter *dbAdapter, QWidget *parent) :
    QDialog(parent)
{
    mDbAdapter = dbAdapter;
    mParseString = parseString;
    mGlossItem = glossItem;
    mAnalysis = MorphologicalAnalysis( mParseString.id(), mParseString.writingSystem() );

    fillMorphologicalAnalysis();
    setupLayout();

    connect( this, SIGNAL(accepted()), this, SLOT(commitChangesToDatabase()));

    setWindowTitle(tr("Choose lexical entries"));
}

MorphologicalAnalysis ChooseLexicalEntriesDialog::morphologicalAnalysis() const
{
    return mAnalysis;
}

void ChooseLexicalEntriesDialog::commitChangesToDatabase()
{
    for(int i=0; i<mAnalysis.allomorphCount(); i++)
    {
        qlonglong allomorphId = mDbAdapter->addAllomorph( mEntries.at(i)->textBit() , mEntries.at(i)->id() );
        mAnalysis[i]->setId(allomorphId);
        mAnalysis[i]->setGlosses( mDbAdapter->lexicalItemGlosses( mEntries.at(i)->id() ) );
    }
    mDbAdapter->setMorphologicalAnalysis( mParseString.id() , mAnalysis );
}

void ChooseLexicalEntriesDialog::fillMorphologicalAnalysis()
{
    mAnalysis = MorphologicalAnalysis( mParseString.id(), mParseString.writingSystem() );

    QStringList bits = mParseString.text().split(QRegExp("\\s"), QString::SkipEmptyParts );
    QStringListIterator iter(bits);
    while(iter.hasNext())
        mAnalysis.addAllomorph( Allomorph( -1, TextBit( iter.next() , mParseString.writingSystem()) ) );
}

void ChooseLexicalEntriesDialog::setupLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    AllomorphIterator iter = mAnalysis.allomorphIterator();
    while(iter.hasNext())
    {
        LexicalEntryForm *form = new LexicalEntryForm( iter.next(), mGlossItem, mDbAdapter, this );
        connect(form, SIGNAL(entryChanged()), this, SLOT(setAcceptable()) );

        layout->addWidget(form);
        mEntries << form;
    }

    mOk = new QPushButton("OK", this);
    QPushButton * cancel = new QPushButton("Cancel", this);
    QPushButton * resegment = new QPushButton("Resegment", this);

    QHBoxLayout *buttons = new QHBoxLayout;
    buttons->addStretch(1);
    buttons->addWidget(mOk);
    buttons->addWidget(cancel);
    buttons->addWidget(resegment);

    connect( mOk, SIGNAL(clicked()), this, SLOT(accept()) );
    connect( cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect( resegment, SIGNAL(clicked()), this, SLOT(reject()));
    connect( resegment, SIGNAL(clicked()), this, SIGNAL(resegment()) );


    layout->addLayout(buttons);

    mOk->setDefault(true);

    setAcceptable();
}

void ChooseLexicalEntriesDialog::setAcceptable()
{
    bool acceptable = true;
    foreach(LexicalEntryForm *form, mEntries)
        if( form->id() == -1 )
            acceptable = false;
    mOk->setEnabled(acceptable);
    mOk->setDefault(true);
}
