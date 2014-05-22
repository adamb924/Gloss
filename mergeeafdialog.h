/*!
  \class MergeEafDialog
  \ingroup GUI
  \ingroup TextImport
  \brief This is a dialog for users who want to merge the sounds and time markers of an EAF file into a text. The user selects the text from a combo box, and a file on the computer. As a convenience the dialog checks whether the base filenames are the same. The actual merge is carried out in MainWindow and Text. The associated interface is mergeeafdialog.ui.
*/

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
