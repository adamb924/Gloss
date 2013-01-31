#ifndef SQLTABLEDIALOG_H
#define SQLTABLEDIALOG_H

#include <QDialog>

namespace Ui {
    class SqlTableDialog;
}

class DatabaseAdapter;

class SqlTableDialog : public QDialog
{
    Q_OBJECT

public:
    SqlTableDialog(const QString & tableName, const DatabaseAdapter * dbAdapter, QWidget *parent = 0);
    ~SqlTableDialog();

private:
    Ui::SqlTableDialog *ui;
};

#endif // SQLTABLEDIALOG_H
