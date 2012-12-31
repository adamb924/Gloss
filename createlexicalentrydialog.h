#ifndef CREATELEXICALENTRYDIALOG_H
#define CREATELEXICALENTRYDIALOG_H

#include <QDialog>
#include <QList>

class DatabaseAdapter;
class GlossItem;
class LingEdit;

#include "allomorph.h"

namespace Ui {
    class CreateLexicalEntryDialog;
}

class CreateLexicalEntryDialog : public QDialog
{
    Q_OBJECT

public:
    CreateLexicalEntryDialog(const Allomorph & allomorph, GlossItem *glossItem, DatabaseAdapter *dbAdapter, QWidget *parent = 0);
    ~CreateLexicalEntryDialog();

    qlonglong id() const;

private:
    Ui::CreateLexicalEntryDialog *ui;
    DatabaseAdapter *mDbAdapter;
    Allomorph mAllomorph;
    GlossItem *mGlossItem;

    qlonglong mId;

    QList<LingEdit*> mGlossEdits;
    QList<LingEdit*> mCitationFormEdits;

private slots:
    void fillData();
    void createLexicalEntry();
};

#endif // CREATELEXICALENTRYDIALOG_H
