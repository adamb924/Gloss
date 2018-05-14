/*!
  \class ExportTextsDialog
  \ingroup GUI
  \brief This is a dialog box for setting options for exporting texts. It is instantiated in MainWindow. The associated UI file is exporttextsdialog.ui.
*/

#ifndef EXPORTTEXTSDIALOG_H
#define EXPORTTEXTSDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class ExportTextsDialog;
}

class ExportTextsDialog : public QDialog
{
    Q_OBJECT

public:
    ExportTextsDialog(QStringList textNames, QWidget *parent = 0);
    ~ExportTextsDialog();

    QStringList textNames() const;
    bool includeGlossNamespace() const;
    QString destinationFolder() const;

private slots:
    void chooseFolder();
    void selectAll();
    void selectNone();
    void onCustomContextMenu(const QPoint &point);
    void checkSelected();
    void uncheckSelected();

private:
    Ui::ExportTextsDialog *ui;
    QStandardItemModel mTextNamesModel;
};

#endif // EXPORTTEXTSDIALOG_H
