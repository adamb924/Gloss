/*!
  \class CreateAnalysisDialog
  \ingroup MorphologicalAnalysis
  \brief This class provides the form in which users type in a text-based morphological analysis (see createanalysisdialog.ui). It's instantiated in AnalysisWidget.
*/


#ifndef CREATEANALYSISDIALOG_H
#define CREATEANALYSISDIALOG_H

#include <QDialog>

namespace Ui {
    class CreateAnalysisDialog;
}

#include "textbit.h"

class DatabaseAdapter;

class CreateAnalysisDialog : public QDialog
{
    Q_OBJECT

public:
    CreateAnalysisDialog(const DatabaseAdapter *dbAdapter, const TextBit & initialString, QWidget *parent = 0);
    ~CreateAnalysisDialog();

    QString analysisString() const;

private slots:
    void acceptSegmentation( const TextBit & textBit );
    void copySegmentation( const TextBit & textBit );

private:
    Ui::CreateAnalysisDialog *ui;
    const DatabaseAdapter *mDbAdapter;
    TextBit mSegmentation;
};

#endif // CREATEANALYSISDIALOG_H
