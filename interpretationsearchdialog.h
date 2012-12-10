#ifndef INTERPRETATIONSEARCHDIALOG_H
#define INTERPRETATIONSEARCHDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

class DatabaseAdapter;

namespace Ui {
    class InterpretationSearchDialog;
}

class InterpretationSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InterpretationSearchDialog(DatabaseAdapter *dbAdapter, QWidget *parent = 0);
    ~InterpretationSearchDialog();

    qlonglong selectionId() const;

private slots:
    void updateQuery();
    void setSelectionRowId();

private:
    void fillWritingSystems();

    qlonglong mSelection;
    QSqlQueryModel mModel;
    DatabaseAdapter *mDbAdapter;
    Ui::InterpretationSearchDialog *ui;
};

#endif // INTERPRETATIONSEARCHDIALOG_H
