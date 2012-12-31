#include "chooselexicalentriesdialog.h"

#include "lexicalentryform.h"

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

}

void ChooseLexicalEntriesDialog::fillMorphologicalAnalysis()
{
    mAnalysis.clear();

    QStringList bits = mParseString.text().split(QRegExp("\\s"), QString::SkipEmptyParts );
    QStringListIterator iter(bits);
    while(iter.hasNext())
        mAnalysis << Allomorph( TextBit( iter.next() , mParseString.writingSystem()) );
}

void ChooseLexicalEntriesDialog::setupLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    MorphologicalAnalysisIterator iter(mAnalysis);
    while(iter.hasNext())
    {
        LexicalEntryForm *form = new LexicalEntryForm( iter.next(), mGlossItem, mDbAdapter, this );
        layout->addWidget(form);
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addWidget(buttonBox);
}
