#ifndef SEARCHREPLACEFORM_H
#define SEARCHREPLACEFORM_H

#include <QWidget>

#include "interlinearitemtype.h"

class DatabaseAdapter;

namespace Ui {
    class SearchReplaceForm;
}

class SearchReplaceForm : public QWidget
{
    Q_OBJECT

public:
    SearchReplaceForm(const QString & checkboxLabel, const InterlinearItemType & type, DatabaseAdapter *dbAdapter, QWidget *parent = 0);
    ~SearchReplaceForm();

    InterlinearItemType type() const;
    qlonglong id() const;

public slots:
    void setFromDatabase( const QString & databaseIndex );
    void setCheckboxChecked(bool enabled);

signals:
    void checkboxChanged(bool enabled);


private:
    Ui::SearchReplaceForm *ui;
    DatabaseAdapter *mDbAdapter;
    InterlinearItemType mType;
};

#endif // SEARCHREPLACEFORM_H
