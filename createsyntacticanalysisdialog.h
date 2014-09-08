#ifndef CREATESYNTACTICANALYSISDIALOG_H
#define CREATESYNTACTICANALYSISDIALOG_H

#include <QDialog>

class WritingSystem;

namespace Ui {
class CreateSyntacticAnalysisDialog;
}

class CreateSyntacticAnalysisDialog : public QDialog
{
    Q_OBJECT

public:
    CreateSyntacticAnalysisDialog(const QList<WritingSystem> & writingSystems, QWidget *parent = 0);
    ~CreateSyntacticAnalysisDialog();

    WritingSystem writingSystem() const;
    QString name() const;
    bool closedVocabulary() const;

private:
    Ui::CreateSyntacticAnalysisDialog *ui;
};

#endif // CREATESYNTACTICANALYSISDIALOG_H
