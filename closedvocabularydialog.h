/*!
  \class ClosedVocabularyDialog
  \ingroup SyntaxGUI
  \brief This is a dialog for editing a set of vocabulary -- the possible syntactic constiuents in a syntactic analysis.

  The associated UI file is closedvocabularydialog.ui.
*/

#ifndef CLOSEDVOCABULARYDIALOG_H
#define CLOSEDVOCABULARYDIALOG_H

#include <QDialog>

class Project;
class QSqlTableModel;

namespace Ui {
class ClosedVocabularyDialog;
}

class ClosedVocabularyDialog : public QDialog
{
    Q_OBJECT

public:
    ClosedVocabularyDialog(Project * prj, QWidget *parent = 0);
    ~ClosedVocabularyDialog();

public slots:
    void accept();
    void reject();

private slots:
    void add();
    void remove();
    void changeRow(const QModelIndex & index);

    void updateDatabaseRecord();
    void validateKeystroke();

private:
    Ui::ClosedVocabularyDialog *ui;
    Project * mProject;
    QSqlTableModel *mModel;
    int mCurrentRow;
};

#endif // CLOSEDVOCABULARYDIALOG_H
