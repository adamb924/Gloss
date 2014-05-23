/*!
  \class WritingSystemsForm
  \ingroup GUI
  \brief This provides an interface for editing the writing systems of a project.
*/

#ifndef WRITINGSYSTEMSFORM_H
#define WRITINGSYSTEMSFORM_H

#include <QWidget>

class DatabaseAdapter;
class QSqlTableModel;

namespace Ui {
class WritingSystemsForm;
}

class WritingSystemsForm : public QWidget
{
    Q_OBJECT

public:
    WritingSystemsForm(DatabaseAdapter *dbAdapter, QWidget *parent = 0);
    ~WritingSystemsForm();

signals:
    void accept();
    void reject();

private slots:
    void insert();
    void remove();

private:
    void setupTable();

    QSqlTableModel *mModel;

    Ui::WritingSystemsForm *ui;
    DatabaseAdapter *mDbAdapter;
};

#endif // WRITINGSYSTEMSFORM_H
