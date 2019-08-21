#ifndef LINGTEXTEDIT_H
#define LINGTEXTEDIT_H

#include <QTextEdit>

#include "textbit.h"

class LingTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit LingTextEdit(QWidget *parent = nullptr);
    explicit LingTextEdit(const TextBit & bit, QWidget *parent = nullptr);

    TextBit textBit() const;

signals:

public slots:
    void setTextBit( const TextBit & bit );
    void setWritingSystem( const WritingSystem & ws );

private:
    WritingSystem mWritingSystem;
    void refreshStyle();

    void focusInEvent ( QFocusEvent * e );
};

#endif // LINGTEXTEDIT_H
