#ifndef GENERICTEXTINPUTDIALOG_H
#define GENERICTEXTINPUTDIALOG_H

#include <QDialog>

class DatabaseAdapter;

#include "writingsystem.h"

namespace Ui {
    class GenericTextInputDialog;
}

class GenericTextInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GenericTextInputDialog(DatabaseAdapter *dbAdapter, QWidget *parent = 0);
    ~GenericTextInputDialog();

    QString text() const;
    WritingSystem writingSystem() const;

private:
    Ui::GenericTextInputDialog *ui;

    QList<WritingSystem> mWritingSystems;

    DatabaseAdapter *mDbAdapter;

    void fillWritingSystems();

private slots:
    void changeCurrentWritingSystem(int index);
};

#endif // GENERICTEXTINPUTDIALOG_H
