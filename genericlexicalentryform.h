#ifndef GENERICLEXICALENTRYFORM_H
#define GENERICLEXICALENTRYFORM_H

#include <QWidget>
#include <QHash>

#include "textbit.h"

namespace Ui {
    class GenericLexicalEntryForm;
}

class GenericLexicalEntryForm : public QWidget
{
    Q_OBJECT

public:
    GenericLexicalEntryForm(const WritingSystem & ws, const QHash<qlonglong,TextBit> & candidates, QWidget *parent = 0);
    ~GenericLexicalEntryForm();

    qlonglong id() const;
    QString text() const;

private:
    Ui::GenericLexicalEntryForm *ui;
};

#endif // GENERICLEXICALENTRYFORM_H
