/*!
  \class LingTextEdit
  \ingroup GUI
  \brief A QTextEdit subclass. It has an extra signal compared to QLineEdit. It also implements focusInEvent(QFocusEvent*) so that when the widget receives focus, the current writing system is changed.
*/

#ifndef LINGTEXTEDIT_H
#define LINGTEXTEDIT_H

#include <QTextEdit>

#include "writingsystem.h"

class LingTextEdit : public QTextEdit
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
