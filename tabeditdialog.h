#ifndef TABEDITDIALOG_H
#define TABEDITDIALOG_H

#include <QDialog>

#include "tab.h"

namespace Ui {
class TabEditDialog;
}

class TabEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TabEditDialog(QWidget *parent = 0);
    ~TabEditDialog();

    Tab::TabType type() const;
    QString name() const;

private:
    Ui::TabEditDialog *ui;
};

#endif // TABEDITDIALOG_H
