#ifndef CLOSEDVOCABULARYDIALOG_H
#define CLOSEDVOCABULARYDIALOG_H

#include <QDialog>
#include <QSqlDatabase>

class Project;
class QSqlQueryModel;
class QSqlTableModel;

namespace Ui {
class ClosedVocabularyDialog;
}

class ClosedVocabularyDialog : public QDialog
{
    Q_OBJECT

public:
    ClosedVocabularyDialog(Project * prj, QWidget *parent = 0);
    ~ClosedVocabularyDialog();

private slots:
    void add();
    void remove();
    void changeRow(const QModelIndex & index);

    void updateDatabaseRecord();
    void validateKeystroke();

private:
    Ui::ClosedVocabularyDialog *ui;
    Project * mProject;
    QSqlTableModel *mModel;
    QSqlDatabase mDatabase;
    int mCurrentRow;
};

#endif // CLOSEDVOCABULARYDIALOG_H
