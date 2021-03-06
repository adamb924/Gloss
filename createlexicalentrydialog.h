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
class Project;
class LexiconLineForm;

#include "allomorph.h"

namespace Ui {
    class CreateLexicalEntryDialog;
}

class CreateLexicalEntryDialog : public QDialog
{
    Q_OBJECT

public:
    CreateLexicalEntryDialog(qlonglong lexicalEntryId, bool hideGuessButton, const GlossItem *glossItem, const Project *project, QWidget *parent = nullptr);
    CreateLexicalEntryDialog(const TextBit & allomorphString, bool hideGuessButton, bool isMonomorphemic, const GlossItem *glossItem, const Project *project, QWidget *parent = nullptr);
    ~CreateLexicalEntryDialog();

    qlonglong lexicalEntryId() const;
    TextBitHash glosses() const;
    QStringList grammaticalTags() const;

signals:
    void linkToOther();

private:
    Ui::CreateLexicalEntryDialog *ui;
    TextBit mAllomorphString;
    const Project * mProject;
    const DatabaseAdapter * mDbAdapter;
    const GlossItem * mGlossItem;
    bool mIsMonomorphemic;

    qlonglong mLexicalEntryId;
    bool mHideGuessButton;

    QList<LexiconLineForm*> mGlossEdits;
    QList<LexiconLineForm*> mCitationFormEdits;

    void addMorphemeTypes();

    Allomorph::Type morphemeType() const;

private slots:
    void guessAppropriateValues();
    void fillFromDatabase();

    void createLexicalEntry();
    void changeLexicalEntry();
};

#endif // CREATELEXICALENTRYDIALOG_H
