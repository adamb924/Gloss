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
    ItemEditDialog(const QList<WritingSystem> & writingSystems, QWidget *parent = 0);
    ~ItemEditDialog();

    InterlinearItemType type() const;
    void setType(InterlinearItemType type);
private:
    Ui::ItemEditDialog *ui;
};

#endif // ITEMEDITDIALOG_H
