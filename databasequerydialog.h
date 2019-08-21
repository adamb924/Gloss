/*!
  \class DatabaseQueryDialog
  \ingroup GUI
  \ingroup RawAccess
  \brief This class provides a form for the user to do SQL queries on the database, and to save the results. See databasequerydialog.ui.
*/

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
    DatabaseQueryDialog(const QString & databaseName, QWidget *parent = nullptr);
    ~DatabaseQueryDialog();

private:
    Ui::DatabaseQueryDialog *ui;
    QString mFilename;
private slots:
    void doQuery();
    void saveCsv();
};

#endif // DATABASEQUERYDIALOG_H
