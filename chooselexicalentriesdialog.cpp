#include "chooselexicalentriesdialog.h"

#include "lexicalentryform.h"
#include "morphologicalanalysis.h"
#include "allomorph.h"
#include "databaseadapter.h"
#include "glossitem.h"
#include "project.h"

#include <QtWidgets>
#include <QtDebug>

ChooseLexicalEntriesDialog::ChooseLexicalEntriesDialog(const TextBit & parseString, const GlossItem *glossItem, Project *project, QWidget *parent) :
    QDialog(parent),
    mProject(project),
    mDbAdapter(mProject->dbAdapter()),
    mParseString(parseString),
    mGlossItem(glossItem)
{
    QStringList pieces = parseString.text().split(" ");
    for(int i=0; i<pieces.count(); i++)
    {
        mAllomorphStrings << TextBit( pieces.at(i) , parseString.writingSystem() );
    }

    QSqlDatabase::database(mDbAdapter->dbFilename()).transaction();

    setupLayout();

    connect( this, SIGNAL(accepted()), this, SLOT(commitChanges()));
    connect( this, SIGNAL(rejected()), this, SLOT(rollbackChanges()) );

    setWindowTitle(tr("Choose lexical entries"));
}

MorphologicalAnalysis * ChooseLexicalEntriesDialog::morphologicalAnalysis() const
{
    MorphologicalAnalysis * analysis = new MorphologicalAnalysis(mParseString.id(), mParseString.writingSystem(), mProject->concordance() );
    for(int i=0; i<mEntries.count(); i++)
    {
        qlonglong allomorphId = mDbAdapter->addAllomorph( mEntries.at(i)->textBit() , mEntries.at(i)->id() );
        Allomorph * allomorph = new Allomorph( allomorphId, mEntries.at(i)->id(), mEntries.at(i)->textBit(), mDbAdapter->lexicalItemGlosses( mEntries.at(i)->id() ), mEntries.at(i)->type() );
        analysis->addAllomorph( allomorph );
    }
    mDbAdapter->setMorphologicalAnalysis( mParseString.id(), analysis);
    return analysis;
}

void ChooseLexicalEntriesDialog::commitChanges()
{
    QSqlDatabase::database(mDbAdapter->dbFilename()).commit();
}

void ChooseLexicalEntriesDialog::rollbackChanges()
{
    QSqlDatabase::database(mDbAdapter->dbFilename()).rollback();
}

void ChooseLexicalEntriesDialog::setupLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    for(int i=0; i<mAllomorphStrings.count(); i++)
    {
        LexicalEntryForm *form = new LexicalEntryForm( mAllomorphStrings.at(i), mGlossItem, mProject, this );
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
