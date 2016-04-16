/*!
  \class GenericTextInputDialog
  \ingroup GUI
  \brief This generic form allows users to enter text for a particular WritingSystem. See generictextinputdialog.ui.
*/

#ifndef GENERICTEXTINPUTDIALOG_H
#define GENERICTEXTINPUTDIALOG_H

#include <QDialog>

class DatabaseAdapter;

#include "textbit.h"

namespace Ui {
    class GenericTextInputDialog;
}

class GenericTextInputDialog : public QDialog
{
    Q_OBJECT

public:
    GenericTextInputDialog(DatabaseAdapter *dbAdapter, QWidget *parent = 0);
    GenericTextInputDialog(const WritingSystem & writingSystem, QWidget *parent = 0);
    GenericTextInputDialog(const TextBit & bit, QWidget *parent = 0);
    ~GenericTextInputDialog();

    void suggestInput( const TextBit & bit );

    QString text() const;
    TextBit textBit() const;
    WritingSystem writingSystem() const;

    void setCursorPosition(int position);

private:
    Ui::GenericTextInputDialog *ui;

    QList<WritingSystem> mWritingSystems;
    WritingSystem mWritingSystem;

    void fillWritingSystems();

private slots:
    void changeCurrentWritingSystem(int index);
};

#endif // GENERICTEXTINPUTDIALOG_H
