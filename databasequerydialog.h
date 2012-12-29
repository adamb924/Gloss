#ifndef DATABASEQUERYDIALOG_H
#define DATABASEQUERYDIALOG_H

#include <QDialog>

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
    QString mFilename;
private slots:
    void doQuery();
};

#endif // DATABASEQUERYDIALOG_H
