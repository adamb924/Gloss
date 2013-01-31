#include "sqltabledialog.h"
#include "ui_sqltabledialog.h"
#include "databaseadapter.h"

#include <QSortFilterProxyModel>

SqlTableDialog::SqlTableDialog(const QString & tableName, const DatabaseAdapter * dbAdapter, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SqlTableDialog)
{
    ui->setupUi(this);

    QSqlTableModel *model = new QSqlTableModel(this,QSqlDatabase::database(dbAdapter->dbFilename()));
    model->setTable(tableName);
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->select();

    QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel( model );
    proxyModel->setFilterKeyColumn(-1);

    connect( ui->filterEdit, SIGNAL(textChanged(QString)), proxyModel, SLOT(setFilterRegExp(QString)));

    ui->tableView->setSortingEnabled(true);
    ui->tableView->setModel(proxyModel);

    setWindowTitle(tableName);
}

SqlTableDialog::~SqlTableDialog()
{
    delete ui;
}
