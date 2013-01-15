#ifndef SINGLEPHRASEEDITDIALOG_H
#define SINGLEPHRASEEDITDIALOG_H

#include <QDialog>

namespace Ui {
    class SinglePhraseEditDialog;
}

class Project;
class DatabaseAdapter;
class Text;
class GlossDisplayWidget;

class SinglePhraseEditDialog : public QDialog
{
    Q_OBJECT

public:
    SinglePhraseEditDialog( int lineNumber, Project *project, Text * text, QWidget *parent = 0);
    ~SinglePhraseEditDialog();

private slots:
    void refreshLayout();

private:
    Ui::SinglePhraseEditDialog *ui;

    int mLineNumber;
    Project *mProject;
    Text *mText;
    GlossDisplayWidget *mGlossDisplayWidget;
};

#endif // SINGLEPHRASEEDITDIALOG_H
