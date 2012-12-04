/*!
  \class NewTextDialog
  \ingroup GUI
  \brief A dialog class for adding a blank interlinear text into the project.
*/

#ifndef NEWTEXTDIALOG_H
#define NEWTEXTDIALOG_H

#include <QDialog>
#include <QList>
#include "writingsystem.h"
#include "project.h"

namespace Ui {
    class Dialog;
}

class NewTextDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewTextDialog( const QList<WritingSystem*> & ws, QWidget *parent = 0);
    ~NewTextDialog();

    QString writingSystem() const;
    QString name() const;

private:
    Ui::Dialog *ui;
};

#endif // NEWTEXTDIALOG_H
