#ifndef LEXICALENTRYSEARCHDIALOG_H
#define LEXICALENTRYSEARCHDIALOG_H

#include <QDialog>

namespace Ui {
    class LexicalEntrySearchDialog;
}

class DatabaseAdapter;
class QStandardItemModel;

#include "writingsystem.h"

class LexicalEntrySearchDialog : public QDialog
{
    Q_OBJECT

public:
    LexicalEntrySearchDialog(const DatabaseAdapter * dbAdapter, QWidget *parent = 0);
    ~LexicalEntrySearchDialog();

    qlonglong lexicalEntryId() const;

private:
    Ui::LexicalEntrySearchDialog *ui;
    const DatabaseAdapter * mDbAdapter;

    QList<WritingSystem> mWritingSystems;
    QStandardItemModel *mFirstModel;
    QStandardItemModel *mSecondModel;
    qlonglong mLexicalEntryId;

private slots:
    void fillCandidates();
    void changeCurrentWritingSystem(int index);
    void activated(const QModelIndex & index);
};

#endif // LEXICALENTRYSEARCHDIALOG_H
