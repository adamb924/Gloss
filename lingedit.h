#ifndef LINGEDIT_H
#define LINGEDIT_H

#include <QLineEdit>

#include "writingsystem.h"
#include "textbit.h"

class LingEdit : public QLineEdit
{
    Q_OBJECT
public:

    explicit LingEdit(const TextBit & bit, QWidget *parent = 0);

private:
    TextBit mTextBit;

    void focusInEvent ( QFocusEvent * e );

signals:
    void stringChanged( const TextBit & );

public slots:
    void textChanged();
};

#endif // LINGEDIT_H
