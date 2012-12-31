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
    LexicalEntryForm(const Allomorph & allomorph, GlossItem *glossItem, DatabaseAdapter *dbAdapter,  QWidget *parent = 0);
    ~LexicalEntryForm();

private:
    Ui::LexicalEntryForm *ui;
    Allomorph mAllomorph;
    DatabaseAdapter *mDbAdapter;
    GlossItem *mGlossItem;

    void fillData();

private slots:
    void newLexicalEntry();
};

#endif // LEXICALENTRYFORM_H
