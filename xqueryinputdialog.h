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
