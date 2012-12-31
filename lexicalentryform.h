#ifndef LEXICALENTRYFORM_H
#define LEXICALENTRYFORM_H

#include <QWidget>

#include "allomorph.h"

namespace Ui {
    class LexicalEntryForm;
}

class DatabaseAdapter;

class LexicalEntryForm : public QWidget
{
    Q_OBJECT

public:
    LexicalEntryForm(const Allomorph & allomorph, DatabaseAdapter *dbAdapter,  QWidget *parent = 0);
    ~LexicalEntryForm();

private:
    Ui::LexicalEntryForm *ui;
    Allomorph mAllomorph;
    DatabaseAdapter *mDbAdapter;

    void fillData();
    void setupLayout();
};

#endif // LEXICALENTRYFORM_H
