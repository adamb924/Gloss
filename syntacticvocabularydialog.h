/*!
  \class ClosedVocabularyDialog
  \ingroup SyntaxGUI
  \brief This is a dialog for editing a set of vocabulary -- the possible syntactic constiuents in a syntactic analysis.

  The associated UI file is syntacticvocabularydialog.ui.
*/

#ifndef CLOSEDVOCABULARYDIALOG_H
#define CLOSEDVOCABULARYDIALOG_H

#include <QDialog>

class Project;
class QSqlTableModel;

namespace Ui {
class SyntacticVocabularyDialog;
}

class SyntacticVocabularyDialog : public QDialog
{
    Q_OBJECT

public:
    SyntacticVocabularyDialog(Project * prj, QWidget *parent = 0);
    ~SyntacticVocabularyDialog();

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
    Ui::SyntacticVocabularyDialog *ui;
    Project * mProject;
    QSqlTableModel *mModel;
    int mCurrentRow;
};

#endif // CLOSEDVOCABULARYDIALOG_H
