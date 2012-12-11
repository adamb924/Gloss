#ifndef WRITINGSYSTEMSDIALOG_H
#define WRITINGSYSTEMSDIALOG_H

#include <QDialog>

class DatabaseAdapter;
class QSqlTableModel;

namespace Ui {
class WritingSystemsDialog;
}

class WritingSystemsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WritingSystemsDialog(DatabaseAdapter *dbAdapter, QWidget *parent = 0);
    ~WritingSystemsDialog();

private slots:
    void insert();
    void remove();

private:
    void setupTable();

    QSqlTableModel *mModel;

    Ui::WritingSystemsDialog *ui;
    DatabaseAdapter *mDbAdapter;
};

#endif // WRITINGSYSTEMSDIALOG_H
