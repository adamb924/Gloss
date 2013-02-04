#include "databasequerydialog.h"
#include "ui_databasequerydialog.h"

#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>

#include "sqlquerywriter.h"

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
    connect(ui->saveCsv, SIGNAL(clicked()), this, SLOT(saveCsv()));

    ui->queryResult->setSortingEnabled(true);

    setWindowTitle(tr("Perform a raw SQL query"));
}

DatabaseQueryDialog::~DatabaseQueryDialog()
{
    delete ui;
}

void DatabaseQueryDialog::doQuery()
{
    mQuery = QSqlQuery(QSqlDatabase::database(mFilename));
    mQuery.prepare(ui->queryEdit->toPlainText());
    mQuery.exec();

    QSqlQueryModel *model = new QSqlQueryModel( ui->queryResult );
    model->setQuery( mQuery );

    QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel( model );
    proxyModel->setFilterKeyColumn(-1);

    ui->queryResult->setModel(proxyModel);
    if( model->query().lastError().type() != QSqlError::NoError )
        QMessageBox::warning(this, tr("Error"), model->query().lastError().text() );
    else
        ui->saveCsv->setEnabled(true);
}

void DatabaseQueryDialog::saveCsv()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save query"), QString(), tr("Comma separated values (*.csv)") );
    if( !filename.isNull() )
    {
        SqlQueryWriter writer( mQuery );
        if( writer.serialize(filename) )
        {
            if( QMessageBox::Yes == QMessageBox::question(this, tr("Gloss"), tr("The report is finished; would you like to open it?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes ) )
                QDesktopServices::openUrl(QUrl(filename, QUrl::TolerantMode));
        }
        else
        {
            QMessageBox::warning(this, tr("Gloss"), tr("The report could not be generated, sorry.") );
        }
    }
}
