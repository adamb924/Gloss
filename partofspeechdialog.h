#ifndef PARTOFSPEECHDIALOG_H
#define PARTOFSPEECHDIALOG_H

#include <QDialog>

namespace Ui {
class PartOfSpeechDialog;
}

class Project;
class QSqlTableModel;

class PartOfSpeechDialog : public QDialog
{
    Q_OBJECT

public:
    PartOfSpeechDialog(const QString & table, Project * prj, QWidget *parent = 0);
    ~PartOfSpeechDialog();

public slots:
    void accept();
    void reject();

private slots:
    void add();
    void remove();
    void changeRow(const QModelIndex & index);

    void updateDatabaseRecord();
    void validateKeystroke();

private:
    Ui::PartOfSpeechDialog *ui;
    Project * mProject;
    QSqlTableModel *mModel;
    int mCurrentRow;
};

#endif // PARTOFSPEECHDIALOG_H
