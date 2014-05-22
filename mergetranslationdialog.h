/*!
  \class MergeTranslationDialog
  \ingroup GUI
  \ingroup TextImport
  \brief This is a dialog for users who want to merge the translations from a .flextext file into a text. The user selects the text from a combo box, a file on the computer, and a writing system. As a convenience the dialog checks whether the base filenames are the same. The actual merge is carried out in MainWindow and Text. The associated interface is mergeeafdialog.ui.
*/

#ifndef MERGETRANSLATIONDIALOG_H
#define MERGETRANSLATIONDIALOG_H

#include <QDialog>

namespace Ui {
    class MergeTranslationDialog;
}

class DatabaseAdapter;
class Project;

#include <QList>
#include "writingsystem.h"

class MergeTranslationDialog : public QDialog
{
    Q_OBJECT

public:
    MergeTranslationDialog(Project *project, QWidget *parent = 0);
    ~MergeTranslationDialog();

    QString text() const;
    WritingSystem writingSystem() const;
    QString filename() const;

private:
    Ui::MergeTranslationDialog *ui;
    DatabaseAdapter *mDbAdapter;
    Project *mProject;
    QList<WritingSystem> mWritingSystems;

private slots:
    void chooseFile();
    void checkParity();
};

#endif // MERGETRANSLATIONDIALOG_H
