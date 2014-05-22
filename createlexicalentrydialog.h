/*!
  \class CreateLexicalEntryDialog
  \ingroup MorphologicalAnalysis
  \brief A form for creating a lexical entry in the database. See createlexicalentrydialog.ui. It is instantiated both in LexicalEntryForm and AnalysisWidget.
*/

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
    CreateLexicalEntryDialog(qlonglong lexicalEntryId, const GlossItem *glossItem, const DatabaseAdapter *dbAdapter, QWidget *parent = 0);
    CreateLexicalEntryDialog(const Allomorph * allomorph, bool isMonomorphemic, const GlossItem *glossItem, const DatabaseAdapter *dbAdapter, QWidget *parent = 0);
    ~CreateLexicalEntryDialog();

    qlonglong lexicalEntryId() const;
    TextBitHash glosses() const;
    QStringList grammaticalTags() const;

signals:
    void linkToOther();

private:
    Ui::CreateLexicalEntryDialog *ui;
    const DatabaseAdapter * mDbAdapter;
    const GlossItem * mGlossItem;
    const Allomorph * mAllomorph;
    bool mIsMonomorphemic;

    qlonglong mLexicalEntryId;

    QList<LingEdit*> mGlossEdits;
    QList<LingEdit*> mCitationFormEdits;

private slots:
    void fillData();
    void guessAppropriateValues();
    void fillFromDatabase();


    void createLexicalEntry();
    void changeLexicalEntry();
};

#endif // CREATELEXICALENTRYDIALOG_H
