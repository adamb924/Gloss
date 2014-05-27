#include "lexicalentrysearchdialog.h"
#include "ui_lexicalentrysearchdialog.h"

#include "databaseadapter.h"
#include "project.h"

#include <QStandardItemModel>

LexicalEntrySearchDialog::LexicalEntrySearchDialog(const Project *project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LexicalEntrySearchDialog),
    mProject(project)
{
    ui->setupUi(this);
    mDbAdapter = mProject->dbAdapter();

    mLexicalEntryId = -1;

    mWritingSystems = mDbAdapter->writingSystems();

    mFirstModel = new QStandardItemModel(this);
    ui->listView->setModel(mFirstModel);

    mSecondModel = new QStandardItemModel(this);
    ui->lowerListView->setModel(mSecondModel);

    ui->writingSystemCombo->setWritingSystems(mDbAdapter->writingSystems());
    ui->writingSystemCombo->setCurrentWritingSystem(mProject->defaultGlossLanguage());

    connect(ui->writingSystemCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeCurrentWritingSystem(int)));
    connect(ui->writingSystemCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(fillCandidates()));

    connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(fillCandidates()));

    connect(ui->listView, SIGNAL(activated(QModelIndex)), this, SLOT(activated(QModelIndex)));
    connect(ui->lowerListView, SIGNAL(activated(QModelIndex)), this, SLOT(activated(QModelIndex)));

    connect(ui->listView, SIGNAL(clicked(QModelIndex)), this, SLOT(activated(QModelIndex)));
    connect(ui->lowerListView, SIGNAL(clicked(QModelIndex)), this, SLOT(activated(QModelIndex)));

    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()));
    connect(ui->lowerListView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

LexicalEntrySearchDialog::~LexicalEntrySearchDialog()
{
    delete ui;
}

qlonglong LexicalEntrySearchDialog::lexicalEntryId() const
{
    return mLexicalEntryId;
}

void LexicalEntrySearchDialog::fillCandidates()
{
    QString searchString = ui->textEdit->text();

    if( searchString.length() < 1 )
    {
        ui->listView->setEnabled(false);
        ui->lowerListView->setEnabled(false);
        return;
    }

    QHash<qlonglong,QString> upper, lower;

    mDbAdapter->searchLexicalEntries( TextBit( searchString, ui->writingSystemCombo->currentWritingSystem() ) , &upper, &lower );

    mFirstModel->clear();
    QHashIterator<qlonglong,QString> iter(upper);
    while(iter.hasNext())
    {
        iter.next();

        QStandardItem *item = new QStandardItem( iter.value() );
        item->setEditable(false);
        item->setData( iter.key(), Qt::UserRole );
        mFirstModel->appendRow(item);
    }
    mFirstModel->sort(0);

    mSecondModel->clear();
    iter = QHashIterator<qlonglong,QString>(lower);
    while(iter.hasNext())
    {
        iter.next();

        QStandardItem *item = new QStandardItem( iter.value() );
        item->setEditable(false);
        item->setData( iter.key(), Qt::UserRole );
        mSecondModel->appendRow(item);
    }
    mSecondModel->sort(0);

    ui->listView->setEnabled(true);
    ui->lowerListView->setEnabled(true);
}

void LexicalEntrySearchDialog::changeCurrentWritingSystem(int index)
{
    ui->textEdit->setWritingSystem( mWritingSystems.at(index) );
}

void LexicalEntrySearchDialog::activated(const QModelIndex & index)
{
    mLexicalEntryId = index.data(Qt::UserRole).toLongLong();
}
