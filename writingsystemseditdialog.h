/*!
  \class WritingSystemsEditDialog
  \ingroup GUI
  \brief This is a dialog for editing the writing systems of a project.

  The associated UI file is writingsystemsdialog.ui.
*/

#ifndef WRITINGSYSTEMSEDITDIALOG_H
#define WRITINGSYSTEMSEDITDIALOG_H

#include <QDialog>

namespace Ui {
class WritingSystemsEditDialog;
}

class Project;
class QSqlTableModel;

class WritingSystemsEditDialog : public QDialog
{
    Q_OBJECT

public:
    WritingSystemsEditDialog(Project * prj, QWidget *parent = 0);
    ~WritingSystemsEditDialog();

public slots:
    void accept();
    void reject();

private slots:
    void add();
    void remove();
    void changeRow(const QModelIndex & index);

    void updateDatabaseRecord();

    void browseKeyboard();
    void browseFont();

private:
    Ui::WritingSystemsEditDialog *ui;

    QSqlTableModel *mModel;
    int mCurrentRow;
};

#endif // WRITINGSYSTEMSEDITDIALOG_H
