/*!
  \class LexicalEntryForm
  \ingroup GUI
  \brief A (piece of a) form for choosing a lexical entry for a particular allomorph. Every ChooseLexicalEntriesDialog has one or more of these.

  The associated form is lexicalentryform.ui
*/

#ifndef LEXICALENTRYFORM_H
#define LEXICALENTRYFORM_H

#include <QWidget>

#include "allomorph.h"

namespace Ui {
    class LexicalEntryForm;
}

class DatabaseAdapter;
class GlossItem;

class LexicalEntryForm : public QWidget
{
    Q_OBJECT

public:
    LexicalEntryForm(const Allomorph & allomorph, const GlossItem *glossItem, const DatabaseAdapter *dbAdapter,  QWidget *parent = 0);
    ~LexicalEntryForm();

    qlonglong id() const;
    TextBit textBit() const;

private:
    Ui::LexicalEntryForm *ui;
    Allomorph mAllomorph;
    const DatabaseAdapter *mDbAdapter;
    const GlossItem *mGlossItem;

    void fillData();

signals:
    void entryChanged();

private slots:
    void newLexicalEntry();
};

#endif // LEXICALENTRYFORM_H
