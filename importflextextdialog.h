/*!
  \class ImportFlexTextDialog
  \ingroup GUI
  \brief A dialog class for importing a FlexText into the project. The layout is importflextextdialog.ui.
*/

#ifndef IMPORTFLEXTEXTDIALOG_H
#define IMPORTFLEXTEXTDIALOG_H

#include <QDialog>

namespace Ui {
    class ImportFlexTextDialog;
}

class DatabaseAdapter;

class ImportFlexTextDialog : public QDialog
{
    Q_OBJECT

public:
    ImportFlexTextDialog(DatabaseAdapter *dbAdapter, QWidget *parent = nullptr);
    ~ImportFlexTextDialog();

    QString filename() const;
    QString writingSystem() const;

private:
    Ui::ImportFlexTextDialog *ui;

    void enable();
    void disable();

    DatabaseAdapter *mDbAdapter;

private slots:
    bool chooseFile();
    void fillDataFromFlexText();
};

#endif // IMPORTFLEXTEXTDIALOG_H
