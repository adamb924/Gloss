/*!
  \class OpenTextDialog
  \ingroup GUI
  \brief A dialog for opening one of the interlinear texts of the project.
*/

#ifndef OPENTEXTDIALOG_H
#define OPENTEXTDIALOG_H

#include <QDialog>

namespace Ui {
class OpenTextDialog;
}

class OpenTextDialog : public QDialog
{
    Q_OBJECT

public:
    OpenTextDialog(const QStringList & textNames, QWidget *parent = nullptr);
    ~OpenTextDialog();

    QString textName() const;
    int linesPerScreen() const;
    int goToLine() const;

private:
    Ui::OpenTextDialog *ui;
};

#endif // OPENTEXTDIALOG_H
