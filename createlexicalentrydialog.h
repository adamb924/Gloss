#ifndef CREATELEXICALENTRYDIALOG_H
#define CREATELEXICALENTRYDIALOG_H

#include <QDialog>

namespace Ui {
    class CreateLexicalEntryDialog;
}

class CreateLexicalEntryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateLexicalEntryDialog(QWidget *parent = 0);
    ~CreateLexicalEntryDialog();

private:
    Ui::CreateLexicalEntryDialog *ui;
};

#endif // CREATELEXICALENTRYDIALOG_H
