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
    WritingSystem mWritingSystem;

    QList<WritingSystem> mWritingSystems;
    void fillWritingSystems();
    QStandardItemModel *mModel;

private slots:
    void fillCandidates( const QString & searchString );
    void changeCurrentWritingSystem(int index);
};

#endif // LEXICALENTRYSEARCHDIALOG_H
