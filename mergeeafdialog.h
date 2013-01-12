#ifndef MERGEEAFDIALOG_H
#define MERGEEAFDIALOG_H

#include <QDialog>

namespace Ui {
    class MergeEafDialog;
}

class Project;

class MergeEafDialog : public QDialog
{
    Q_OBJECT

public:
    MergeEafDialog(Project *project, QWidget *parent = 0);
    ~MergeEafDialog();

    QString text() const;
    QString filename() const;

private:
    Ui::MergeEafDialog *ui;

private slots:
    void chooseFile();
    void checkParity();
};

#endif // MERGEEAFDIALOG_H
