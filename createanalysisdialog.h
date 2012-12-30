#ifndef CREATEANALYSISDIALOG_H
#define CREATEANALYSISDIALOG_H

#include <QDialog>

namespace Ui {
    class CreateAnalysisDialog;
}

class TextBit;

class CreateAnalysisDialog : public QDialog
{
    Q_OBJECT

public:
    CreateAnalysisDialog(const TextBit & initialString, QWidget *parent = 0);
    ~CreateAnalysisDialog();

    QString analysisString() const;

private:
    Ui::CreateAnalysisDialog *ui;
};

#endif // CREATEANALYSISDIALOG_H
