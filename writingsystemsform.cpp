#include "writingsystemsform.h"
#include "ui_writingsystemsform.h"

#include <QSqlTableModel>
#include "databaseadapter.h"

WritingSystemsForm::WritingSystemsForm(DatabaseAdapter *dbAdapter, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WritingSystemsForm)
{
    ui->setupUi(this);

    mDbAdapter = dbAdapter;

    mModel = new QSqlTableModel(this, QSqlDatabase::database(mDbAdapter->dbFilename()));
    setupTable();

    setWindowTitle("Writing Systems");

    connect( ui->submit, SIGNAL(clicked()), mModel, SLOT(submitAll()));
    connect( ui->submit, SIGNAL(clicked()), this, SIGNAL(accept()) );
    connect( ui->dismiss, SIGNAL(clicked()), this, SIGNAL(accept()) );
    connect( ui->insertRow, SIGNAL(clicked()), this, SLOT(insert()));
    connect( ui->deleteRow, SIGNAL(clicked()), this, SLOT(remove()));
}

WritingSystemsForm::~WritingSystemsForm()
{
    delete mModel;
    delete ui;
}

void WritingSystemsForm::setupTable()
{
    mModel->setTable("WritingSystems");
    mModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    mModel->select();
    mModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
    mModel->setHeaderData(2, Qt::Horizontal, tr("Abbreviation"));
    mModel->setHeaderData(3, Qt::Horizontal, tr("Flex String"));
    mModel->setHeaderData(4, Qt::Horizontal, tr("Keyboard Switch File"));
    mModel->setHeaderData(5, Qt::Horizontal, tr("Text Direction"));
    mModel->setHeaderData(6, Qt::Horizontal, tr("Font Family"));
    mModel->setHeaderData(7, Qt::Horizontal, tr("Font Size"));

    ui->tableView->setModel(mModel);
    ui->tableView->hideColumn(0); // don't show the ID
}

void WritingSystemsForm::insert()
{
    mModel->insertRow(0);
}

void WritingSystemsForm::remove()
{
    mModel->removeRow( ui->tableView->selectionModel()->selection().indexes().first().row() );
}
