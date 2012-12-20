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
    explicit MergeTranslationDialog(Project *project, QWidget *parent = 0);
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
