#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>
#include <QList>

#include "interlinearitemtype.h"
#include "searchreplaceform.h"

namespace Ui {
    class ReplaceDialog;
}

class DatabaseAdapter;

class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    ReplaceDialog(DatabaseAdapter *dbAdapter, QWidget *parent = 0);
    ~ReplaceDialog();

    QString xmlFilePath() const;

private slots:
    void generateXsl();

private:
    void setupLayout();

    QList<SearchReplaceForm*> mSearchForms;
    QList<SearchReplaceForm*> mReplaceForms;

    Ui::ReplaceDialog *ui;
    DatabaseAdapter *mDbAdapter;
    QList<InterlinearItemType> mInterlinearLines;
    QString mXmlFilePath;
};

#endif // REPLACEDIALOG_H
