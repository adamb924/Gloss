#include "interpretationsearchdialog.h"
#include "ui_interpretationsearchdialog.h"

#include "databaseadapter.h"

InterpretationSearchDialog::InterpretationSearchDialog(DatabaseAdapter *dbAdapter, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InterpretationSearchDialog)
{
    ui->setupUi(this);
    mDbAdapter = dbAdapter;
    fillWritingSystems();

    ui->resultList->setModel(&mModel);
    ui->resultList->setSelectionMode(QAbstractItemView::SingleSelection);

    mSelection = -1;

    connect( ui->searchString , SIGNAL(textChanged(QString)), this, SLOT(updateQuery()) );
    connect( ui->writingSystem , SIGNAL(currentIndexChanged(int)), this, SLOT(updateQuery()) );
    connect( this, SIGNAL(accepted()), this, SLOT(setSelectionRowId()) );
}

InterpretationSearchDialog::~InterpretationSearchDialog()
{
    delete ui;
}

qlonglong InterpretationSearchDialog::selectionId() const
{
    return mSelection;
}

void InterpretationSearchDialog::updateQuery()
{
    QString searchString = ui->searchString->text();
    qlonglong ws = ui->writingSystem->itemData( ui->writingSystem->currentIndex() ).toLongLong();
    if( searchString.length() > 1 )
    {
        QString query = QString("select InterpretationId,group_concat(Form,', ') from ( select InterpretationId, Form from TextForms where InterpretationId in (select InterpretationId from TextForms where substr(Form,1,length('%2'))='%2' and WritingSystem='%1') union select InterpretationId, Form from Glosses where InterpretationId in (select InterpretationId from TextForms where substr(Form,1,length('%2'))='%2' and WritingSystem='%1') ) group by InterpretationId;").arg(ws).arg(DatabaseAdapter::sqlEscape(searchString));
        mModel.setQuery(query, QSqlDatabase::database(mDbAdapter->dbFilename()) );
        ui->resultList->setColumnHidden(0, true);
    }
}

void InterpretationSearchDialog::fillWritingSystems()
{
    QList<WritingSystem> writingSystems = mDbAdapter->writingSystems();
    foreach( WritingSystem ws , writingSystems )
        ui->writingSystem->addItem( ws.name() , ws.id() );
}

void InterpretationSearchDialog::setSelectionRowId()
{
    QModelIndexList indexes = ui->resultList->selectionModel()->selection().indexes();
    if( indexes.count() == 1 )
    {
        int row = indexes.first().row();
        mSelection = mModel.record(row).value(0).toLongLong();
    }
    else
    {
        mSelection = -1;
    }
}
