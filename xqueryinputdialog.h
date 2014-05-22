/*!
  \class XQueryInputDialog
  \ingroup GUI
  \ingroup RawAccess
  \brief A form in which the user enters an XQuery to be applied to all texts. See xqueryinputdialog.ui. Instantiated in MainWindow, in which the results are displayed in a dock.
*/

#ifndef XQUERYINPUTDIALOG_H
#define XQUERYINPUTDIALOG_H

#include <QDialog>

namespace Ui {
    class XQueryInputDialog;
}

class XQueryInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit XQueryInputDialog(QWidget *parent = 0);
    ~XQueryInputDialog();

    QString query() const;

private:
    Ui::XQueryInputDialog *ui;
};

#endif // XQUERYINPUTDIALOG_H
