/*!
  \class SyntacticAnalysisOptionsDialog
  \ingroup SyntaxGUI
  \brief This class provides a dialog for changing the settings of a SyntacticAnalysis object. It is instantiated in SyntacticParsingWidget.

  The associated UI file is syntacticanalysisoptionsdialog.ui.
*/

#ifndef CREATESYNTACTICANALYSISDIALOG_H
#define CREATESYNTACTICANALYSISDIALOG_H

#include <QDialog>

class WritingSystem;
class SyntacticAnalysis;

namespace Ui {
class SyntacticAnalysisOptionsDialog;
}

class SyntacticAnalysisOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    SyntacticAnalysisOptionsDialog(const QList<WritingSystem> & writingSystems, QWidget *parent = 0);
    SyntacticAnalysisOptionsDialog(const SyntacticAnalysis * analysis, const QList<WritingSystem> & writingSystems, QWidget *parent = 0);
    ~SyntacticAnalysisOptionsDialog();

    WritingSystem writingSystem() const;
    QString name() const;
    bool closedVocabulary() const;

private:
    Ui::SyntacticAnalysisOptionsDialog *ui;
};

#endif // CREATESYNTACTICANALYSISDIALOG_H
