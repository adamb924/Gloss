/*!
  \class WritingSystemDialog
  \ingroup GUI
  \brief This is a dialog box containing a WritingSystemCombo widget. The associated UI file is writingsystemdialog.ui.
*/

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
    WritingSystemDialog(const QList<WritingSystem> & writingSystems, QWidget *parent = 0);
    ~WritingSystemDialog();

    WritingSystem writingSystem() const;
    void setCurrentWritingSystem(const WritingSystem & ws);

private:
    Ui::WritingSystemDialog *ui;
};

#endif // WRITINGSYSTEMDIALOG_H
