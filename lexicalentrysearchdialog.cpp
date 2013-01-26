#include "lexicalentrysearchdialog.h"
#include "ui_lexicalentrysearchdialog.h"

#include "databaseadapter.h"

#include <QStandardItemModel>

LexicalEntrySearchDialog::LexicalEntrySearchDialog(const DatabaseAdapter * dbAdapter, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LexicalEntrySearchDialog)
{
    ui->setupUi(this);
    mDbAdapter = dbAdapter;

    mWritingSystems = dbAdapter->writingSystems();

    mModel = new QStandardItemModel;
    ui->listView->setModel(mModel);

    connect(ui->writingSystemCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeCurrentWritingSystem(int)));
    connect(ui->textEdit, SIGNAL(textChanged(QString)), this, SLOT(fillCandidates(QString)));

    fillWritingSystems();
}

LexicalEntrySearchDialog::~LexicalEntrySearchDialog()
{
    delete ui;
}

qlonglong LexicalEntrySearchDialog::lexicalEntryId() const
{
    if( ui->listView->model()->rowCount() > 0 )
        return ui->listView->currentIndex().data(Qt::UserRole).toLongLong();
    else
        return -1;
}

void LexicalEntrySearchDialog::fillWritingSystems()
{
    for(int i=0; i<mWritingSystems.count(); i++)
        ui->writingSystemCombo->addItem( tr("%1 (%2)").arg(mWritingSystems.at(i).name()).arg(mWritingSystems.at(i).flexString()), i );
}

void LexicalEntrySearchDialog::fillCandidates( const QString & searchString )
{
    if( searchString.length() < 3 )
    {
        ui->listView->setEnabled(false);
        return;
    }

    mModel->clear();
    QHash<qlonglong,QString> candidates = mDbAdapter->searchLexicalEntries( TextBit( searchString, mWritingSystem ) );
    QHashIterator<qlonglong,QString> iter(candidates);

    while(iter.hasNext())
    {
        iter.next();

        QStandardItem *item = new QStandardItem( iter.value() );
        item->setEditable(false);
        item->setData( iter.key(), Qt::UserRole );
        mModel->appendRow(item);
    }

    ui->listView->setEnabled(true);
}

void LexicalEntrySearchDialog::changeCurrentWritingSystem(int index)
{
    ui->textEdit->setWritingSystem( mWritingSystems.at(index) );
    mWritingSystem = mWritingSystems.at(index);
}
