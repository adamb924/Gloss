#ifndef SINGLEPHRASEEDITDIALOG_H
#define SINGLEPHRASEEDITDIALOG_H

#include <QDialog>

namespace Ui {
    class SinglePhraseEditDialog;
}

#include <QList>

#include "focus.h"

class Project;
class DatabaseAdapter;
class Text;

class SinglePhraseEditDialog : public QDialog
{
    Q_OBJECT

public:
    SinglePhraseEditDialog( QList<int> lines, Project *project, Text * text, const QList<Focus> & foci = QList<Focus>(),  QWidget *parent = 0);
    ~SinglePhraseEditDialog();

private:
    Ui::SinglePhraseEditDialog *ui;

    void setTitle();

    QList<int> mLines;
    Project *mProject;
    Text *mText;
};

#endif // SINGLEPHRASEEDITDIALOG_H
