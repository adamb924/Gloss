#include "databasequerydialog.h"
#include "ui_databasequerydialog.h"

#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QSortFilterProxyModel>

DatabaseQueryDialog::DatabaseQueryDialog(const QString & databaseName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseQueryDialog)
{
    mFilename = databaseName;

    QSqlDatabase db = QSqlDatabase::database(mFilename);
    db.setHostName("hostname");
    db.setDatabaseName(mFilename);

    ui->setupUi(this);

    connect(ui->doQuery, SIGNAL(clicked()), this, SLOT(doQuery()));

    ui->queryResult->setSortingEnabled(true);

    setWindowTitle(tr("Perform a raw SQL query"));
}

DatabaseQueryDialog::~DatabaseQueryDialog()
{
    delete ui;
}

void DatabaseQueryDialog::doQuery()
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare(ui->queryEdit->toPlainText());
    q.exec();

    QSqlQueryModel *model = new QSqlQueryModel( ui->queryResult );
    model->setQuery( q );

    QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel( model );
    proxyModel->setFilterKeyColumn(-1);

    ui->queryResult->setModel(proxyModel);
    if( model->query().lastError().type() != QSqlError::NoError )
        QMessageBox::warning(this, tr("Error"), model->query().lastError().text() );
}
