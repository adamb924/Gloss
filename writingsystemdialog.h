#ifndef WRITINGSYSTEMDIALOG_H
#define WRITINGSYSTEMDIALOG_H

#include <QDialog>
#include "writingsystem.h"

namespace Ui {
class WritingSystemDialog;
}

class WritingSystemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WritingSystemDialog(const QList<WritingSystem> & writingSystems, QWidget *parent = 0);
    ~WritingSystemDialog();

    WritingSystem writingSystem() const;

private:
    Ui::WritingSystemDialog *ui;
};

#endif // WRITINGSYSTEMDIALOG_H
