#include "writingsystemsdialog.h"
#include "ui_writingsystemsdialog.h"

#include <QSqlTableModel>
#include "databaseadapter.h"

WritingSystemsDialog::WritingSystemsDialog(DatabaseAdapter *dbAdapter, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WritingSystemsDialog)
{
    ui->setupUi(this);

    mDbAdapter = dbAdapter;

    mModel = new QSqlTableModel(this, QSqlDatabase::database(mDbAdapter->dbFilename()));
    setupTable();

    setWindowTitle("Writing Systems");

    connect( ui->submit, SIGNAL(clicked()), mModel, SLOT(revertAll()));
    connect( ui->revert, SIGNAL(clicked()), mModel, SLOT(submitAll()));
    connect( ui->dismiss, SIGNAL(clicked()), this, SLOT(accept()) );
    connect( ui->insertRow, SIGNAL(clicked()), this, SLOT(insert()));
    connect( ui->deleteRow, SIGNAL(clicked()), this, SLOT(remove()));
}

WritingSystemsDialog::~WritingSystemsDialog()
{
    delete mModel;
    delete ui;
}

void WritingSystemsDialog::setupTable()
{
    mModel->setTable("WritingSystems");
    mModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    mModel->select();
    mModel->removeColumn(0); // don't show the ID
    mModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    mModel->setHeaderData(1, Qt::Horizontal, tr("Abbreviation"));
    mModel->setHeaderData(2, Qt::Horizontal, tr("Flex String"));
    mModel->setHeaderData(3, Qt::Horizontal, tr("Keyboard Switch File"));
    mModel->setHeaderData(4, Qt::Horizontal, tr("Text Direction"));
    mModel->setHeaderData(5, Qt::Horizontal, tr("Font Family"));
    mModel->setHeaderData(6, Qt::Horizontal, tr("Font Size"));

    ui->tableView->setModel(mModel);
}

void WritingSystemsDialog::insert()
{
    mModel->insertRow(0);
}

void WritingSystemsDialog::remove()
{
    mModel->removeRow( ui->tableView->selectionModel()->selection().indexes().first().row() );
}
