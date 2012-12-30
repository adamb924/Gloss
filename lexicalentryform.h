#ifndef LEXICALENTRYFORM_H
#define LEXICALENTRYFORM_H

#include <QWidget>

#include "allomorph.h"

namespace Ui {
    class LexicalEntryForm;
}

class LexicalEntryForm : public QWidget
{
    Q_OBJECT

public:
    LexicalEntryForm(const Allomorph & allomorph, QWidget *parent = 0);
    ~LexicalEntryForm();

private:
    Ui::LexicalEntryForm *ui;
    Allomorph mAllomorph;

    void fillData();
};

#endif // LEXICALENTRYFORM_H
