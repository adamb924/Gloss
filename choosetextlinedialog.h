#ifndef CHOOSETEXTLINEDIALOG_H
#define CHOOSETEXTLINEDIALOG_H

#include <QDialog>

namespace Ui {
    class ChooseTextLineDialog;
}

class ChooseTextLineDialog : public QDialog
{
    Q_OBJECT

public:
    ChooseTextLineDialog(const QStringList & textNames, QWidget *parent = 0);
    ~ChooseTextLineDialog();

    QString textName() const;
    int lineNumber() const;

private:
    Ui::ChooseTextLineDialog *ui;
};

#endif // CHOOSETEXTLINEDIALOG_H
