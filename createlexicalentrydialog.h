#ifndef CREATELEXICALENTRYDIALOG_H
#define CREATELEXICALENTRYDIALOG_H

#include <QDialog>
#include <QList>

class DatabaseAdapter;
class GlossItem;
class LingEdit;
class Allomorph;

#include "allomorph.h"

namespace Ui {
    class CreateLexicalEntryDialog;
}

class CreateLexicalEntryDialog : public QDialog
{
    Q_OBJECT

public:
    CreateLexicalEntryDialog(const Allomorph * allomorph, bool isMonomorphemic, const GlossItem *glossItem, const DatabaseAdapter *dbAdapter, QWidget *parent = 0);
    ~CreateLexicalEntryDialog();

    qlonglong id() const;
    TextBitHash glosses() const;
    QStringList grammaticalTags() const;

private:
    Ui::CreateLexicalEntryDialog *ui;
    const DatabaseAdapter *mDbAdapter;
    const GlossItem *mGlossItem;
    const Allomorph * mAllomorph;
    bool mIsMonomorphemic;

    qlonglong mId;

    QList<LingEdit*> mGlossEdits;
    QList<LingEdit*> mCitationFormEdits;

private slots:
    void fillData();
    void createLexicalEntry();
};

#endif // CREATELEXICALENTRYDIALOG_H
