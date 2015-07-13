/*!
  \class LexicalEntryForm
  \ingroup MorphologicalAnalysis
  \brief A (piece of a) form for choosing a lexical entry for a particular allomorph. Every ChooseLexicalEntriesDialog has one or more of these.

  The associated form is lexicalentryform.ui
*/

#ifndef LEXICALENTRYFORM_H
#define LEXICALENTRYFORM_H

#include <QWidget>

#include "allomorph.h"

#include <QSet>

namespace Ui {
    class LexicalEntryForm;
}

class DatabaseAdapter;
class GlossItem;
class Project;

class LexicalEntryForm : public QWidget
{
    Q_OBJECT

public:
    LexicalEntryForm(const TextBit & allomorphString, const GlossItem *glossItem, const Project *project,  QWidget *parent = 0);
    ~LexicalEntryForm();

    qlonglong id() const;
    TextBit textBit() const;
    Allomorph::Type type() const;

private:
    Ui::LexicalEntryForm *ui;
    TextBit mAllomorphText;
    Allomorph::Type mAllomorphType;
    const DatabaseAdapter *mDbAdapter;
    const Project *mProject;
    const GlossItem *mGlossItem;
    QSet<Allomorph::Type> mTypes;

    void fillTypes();
    void fillData(qlonglong currentLexicalEntryId = -1);

signals:
    void entryChanged();

private slots:
    void newLexicalEntry();
    void linkToOther();
    void quickLE();
    void setType(int index);
};

#endif // LEXICALENTRYFORM_H
