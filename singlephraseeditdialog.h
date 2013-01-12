#ifndef SINGLEPHRASEEDITDIALOG_H
#define SINGLEPHRASEEDITDIALOG_H

#include <QDialog>

namespace Ui {
    class SinglePhraseEditDialog;
}

class Phrase;
class DatabaseAdapter;
class Text;

class SinglePhraseEditDialog : public QDialog
{
    Q_OBJECT

public:
    SinglePhraseEditDialog( int index, Phrase *phrase, Text * text, DatabaseAdapter *dbAdapter,  QWidget *parent = 0);
    ~SinglePhraseEditDialog();

private:
    Ui::SinglePhraseEditDialog *ui;
};

#endif // SINGLEPHRASEEDITDIALOG_H
