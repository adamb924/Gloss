#ifndef CLOSEDVOCABULARYDIALOG_H
#define CLOSEDVOCABULARYDIALOG_H

#include <QDialog>

class Project;

namespace Ui {
class ClosedVocabularyDialog;
}

class ClosedVocabularyDialog : public QDialog
{
    Q_OBJECT

public:
    ClosedVocabularyDialog(Project * prj, QWidget *parent = 0);
    ~ClosedVocabularyDialog();

private:
    Ui::ClosedVocabularyDialog *ui;
    Project * mProject;
};

#endif // CLOSEDVOCABULARYDIALOG_H
