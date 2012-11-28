#ifndef IMPORTFLEXTEXTDIALOG_H
#define IMPORTFLEXTEXTDIALOG_H

#include "project.h"

#include <QDialog>

namespace Ui {
    class ImportFlexTextDialog;
}

class Project;

class ImportFlexTextDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportFlexTextDialog(Project *project, QWidget *parent = 0);
    ~ImportFlexTextDialog();

    QString filename() const;
    QString writingSystem() const;
    Text::BaselineMode baselineMode() const;

private:
    Ui::ImportFlexTextDialog *ui;

    void enable();
    void disable();

    Project *mProject;

private slots:
    void chooseFile();
    void fillDataFromFlexText();
};

#endif // IMPORTFLEXTEXTDIALOG_H
