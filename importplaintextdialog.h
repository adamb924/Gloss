#ifndef IMPORTPLAINTEXTDIALOG_H
#define IMPORTPLAINTEXTDIALOG_H

#include <QDialog>

#include <QList>
#include "writingsystem.h"

namespace Ui {
class ImportPlainTextDialog;
}

class ImportPlainTextDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportPlainTextDialog(const QList<WritingSystem> & writingSystems, QWidget *parent = 0);
    ~ImportPlainTextDialog();

    QStringList filenames() const;
    WritingSystem writingSystem() const;
    bool customDelimiter() const;
    QRegularExpression delimiter() const;

private slots:
    void chooseFilename();
    void delimiterChecked(bool checked);

private:
    Ui::ImportPlainTextDialog *ui;

    void accept();
};

#endif // IMPORTPLAINTEXTDIALOG_H
