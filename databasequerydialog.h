#ifndef DATABASEQUERYDIALOG_H
#define DATABASEQUERYDIALOG_H

#include <QDialog>
#include <QSqlQuery>

namespace Ui {
    class DatabaseQueryDialog;
}

class DatabaseQueryDialog : public QDialog
{
    Q_OBJECT

public:
    DatabaseQueryDialog(const QString & databaseName, QWidget *parent = 0);
    ~DatabaseQueryDialog();

private:
    Ui::DatabaseQueryDialog *ui;
    QSqlQuery mQuery;
    QString mFilename;
private slots:
    void doQuery();
    void saveCsv();
};

#endif // DATABASEQUERYDIALOG_H
