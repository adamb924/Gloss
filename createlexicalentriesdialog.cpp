#include "createlexicalentriesdialog.h"

#include "lexicalentryform.h"

#include <QtGui>
#include <QtDebug>

CreateLexicalEntriesDialog::CreateLexicalEntriesDialog(const TextBit & parseString, DatabaseAdapter *dbAdapter, QWidget *parent) :
    QDialog(parent)
{
    mDbAdapter = dbAdapter;
    mParseString = parseString;

    fillMorphologicalAnalysis();
    setupLayout();

    connect( this, SIGNAL(accepted()), this, SLOT(commitChangesToDatabase()));

    setWindowTitle(tr("Create lexical entries"));
}

void CreateLexicalEntriesDialog::commitChangesToDatabase()
{

}

void CreateLexicalEntriesDialog::fillMorphologicalAnalysis()
{
    mAnalysis.clear();

    QStringList bits = mParseString.text().split(QRegExp("\\s"), QString::SkipEmptyParts );
    QStringListIterator iter(bits);
    while(iter.hasNext())
        mAnalysis << Allomorph( TextBit( iter.next() , mParseString.writingSystem()) );
}

void CreateLexicalEntriesDialog::setupLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    MorphologicalAnalysisIterator iter(mAnalysis);
    while(iter.hasNext())
    {
        LexicalEntryForm *form = new LexicalEntryForm( iter.next(), mDbAdapter, this );
        layout->addWidget(form);
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addWidget(buttonBox);
}
