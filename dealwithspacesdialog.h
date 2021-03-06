/*!
  \class DealWithSpacesDialog
  \ingroup GUI
  \ingroup TextImport
  \brief A form in which the user selects how to interpret spaces in the imported text. See dealwithspacesdialog.ui. Instantiated in WordDisplayWidget.
*/

#ifndef DEALWITHSPACESDIALOG_H
#define DEALWITHSPACESDIALOG_H

#include <QDialog>

namespace Ui {
    class DealWithSpacesDialog;
}

class DealWithSpacesDialog : public QDialog
{
    Q_OBJECT

public:
    enum Result { SplitWord, ConvertSpaces, CancelEdit };

    explicit DealWithSpacesDialog(QWidget *parent = nullptr);
    ~DealWithSpacesDialog();

    DealWithSpacesDialog::Result choice() const;

private slots:
    void splitWord();
    void convertSpaces();
    void cancelEdit();

private:
    Ui::DealWithSpacesDialog *ui;

    Result mResult;
};

#endif // DEALWITHSPACESDIALOG_H
