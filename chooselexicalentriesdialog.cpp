#include "chooselexicalentriesdialog.h"

#include "lexicalentryform.h"
#include "morphologicalanalysis.h"
#include "allomorph.h"
#include "databaseadapter.h"
#include "glossitem.h"

#include <QtGui>
#include <QtDebug>

ChooseLexicalEntriesDialog::ChooseLexicalEntriesDialog(const TextBit & parseString, GlossItem *glossItem, DatabaseAdapter *dbAdapter, QWidget *parent) :
    QDialog(parent)
{
    mDbAdapter = dbAdapter;
    mParseString = parseString;
    mGlossItem = glossItem;

    fillMorphologicalAnalysis();
    setupLayout();

    connect( this, SIGNAL(accepted()), this, SLOT(commitChangesToDatabase()));

    setWindowTitle(tr("Choose lexical entries"));
}

void ChooseLexicalEntriesDialog::commitChangesToDatabase()
{
    Q_ASSERT( mEntries.count() == mAnalysis.count() );
    mGlossItem->morphologicalAnalysis(mParseString.writingSystem())->clear();
    for(int i=0; i<mAnalysis.count(); i++)
    {
        qlonglong allomorphId = mDbAdapter->addAllomorph( mEntries.at(i)->textBit() , mEntries.at(i)->id() );
        mAnalysis[i].setId(allomorphId);
        mAnalysis[i].setGlosses( mDbAdapter->lexicalItemGlosses( mEntries.at(i)->id() ) );
        mGlossItem->addAllomorphToAnalysis( mAnalysis.at(i), mParseString.writingSystem() );
    }
    mDbAdapter->setMorphologicalAnalysis( mGlossItem->textForm( mParseString.writingSystem() ).id() , mAnalysis );
}

void ChooseLexicalEntriesDialog::fillMorphologicalAnalysis()
{
    mAnalysis.clear();

    QStringList bits = mParseString.text().split(QRegExp("\\s"), QString::SkipEmptyParts );
    QStringListIterator iter(bits);
    while(iter.hasNext())
        mAnalysis << Allomorph( -1, TextBit( iter.next() , mParseString.writingSystem()) );
}

void ChooseLexicalEntriesDialog::setupLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    MorphologicalAnalysisIterator iter(mAnalysis);
    while(iter.hasNext())
    {
        LexicalEntryForm *form = new LexicalEntryForm( iter.next(), mGlossItem, mDbAdapter, this );
        connect(form, SIGNAL(entryChanged()), this, SLOT(setAcceptable()) );

        layout->addWidget(form);
        mEntries << form;
    }

    mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(mButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(mButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addWidget(mButtonBox);

    setAcceptable();
}

void ChooseLexicalEntriesDialog::setAcceptable()
{
    bool acceptable = true;
    foreach(LexicalEntryForm *form, mEntries)
        if( form->id() == -1 )
            acceptable = false;
    mButtonBox->button(QDialogButtonBox::Ok)->setEnabled(acceptable);
}
