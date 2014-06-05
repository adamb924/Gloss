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
    ui(new Ui::DatabaseQueryDialog),
    mFilename(databaseName)
{
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
    QSqlQueryModel *model = new QSqlQueryModel( ui->queryResult );
    model->setQuery( ui->queryEdit->toPlainText() , QSqlDatabase::database(mFilename) );

    QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel( model );
    proxyModel->setFilterKeyColumn(-1);

    ui->queryResult->setModel(proxyModel);
    if( model->query().lastError().type() != QSqlError::NoError )
        QMessageBox::warning(this, tr("Error"), model->lastError().text() );
    else
        ui->saveCsv->setEnabled(true);
}

void DatabaseQueryDialog::saveCsv()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save query"), QString(), tr("Comma separated values (*.csv)") );
    if( !filename.isNull() )
    {
        SqlQueryWriter writer( QSqlQuery(ui->queryEdit->toPlainText() , QSqlDatabase::database(mFilename)) );
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
