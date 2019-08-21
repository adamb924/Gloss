/*!
  \class ItemEditDialog
  \ingroup GUI
  \brief This is a dialog for editing an InterlinearItemType. It's currently used in ViewConfigurationDialog. The associated UI file is itemeditdialog.h.
*/

#ifndef ITEMEDITDIALOG_H
#define ITEMEDITDIALOG_H

#include <QDialog>

class InterlinearItemType;
class WritingSystem;

namespace Ui {
class ItemEditDialog;
}

class ItemEditDialog : public QDialog
{
    Q_OBJECT

public:
    ItemEditDialog(const QList<WritingSystem> & writingSystems, QWidget *parent = nullptr);
    ~ItemEditDialog();

    InterlinearItemType type() const;
    void setType(InterlinearItemType type);

    WritingSystem writingSystem() const;
    void setWritingSystem(const WritingSystem & ws, bool enabled = true);
private:
    Ui::ItemEditDialog *ui;
};

#endif // ITEMEDITDIALOG_H
