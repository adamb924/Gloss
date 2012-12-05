#ifndef LINGTEXTEDIT_H
#define LINGTEXTEDIT_H

#include <QPlainTextEdit>

#include "writingsystem.h"

class LingTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit LingTextEdit(QWidget *parent = 0);

    void setWritingSystem(const WritingSystem & ws);

signals:

public slots:

private:
    WritingSystem mWritingSystem;

    void focusInEvent ( QFocusEvent * e );
};

#endif // LINGTEXTEDIT_H
