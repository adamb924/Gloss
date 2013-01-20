#ifndef SINGLEPHRASEEDITDIALOG_H
#define SINGLEPHRASEEDITDIALOG_H

#include <QDialog>

namespace Ui {
    class SinglePhraseEditDialog;
}

#include <QList>

class Project;
class DatabaseAdapter;
class Text;

class SinglePhraseEditDialog : public QDialog
{
    Q_OBJECT

public:
    SinglePhraseEditDialog( QList<int> lines, Project *project, Text * text, QWidget *parent = 0);
    ~SinglePhraseEditDialog();

private:
    Ui::SinglePhraseEditDialog *ui;

    void setTitle();

    QList<int> mLines;
    Project *mProject;
    Text *mText;
};

#endif // SINGLEPHRASEEDITDIALOG_H
