/*!
  \class LingEdit
  \ingroup GUI
  \brief A QLineEdit subclass. It has an extra signal compared to QLineEdit. It also implements focusInEvent(QFocusEvent*) so that when the widget receives focus, the current writing system is changed.
*/

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

    qlonglong id() const;

    void setTextBit( const TextBit & bit );

    void matchTextAlignmentTo( Qt::LayoutDirection target );

private:
    TextBit mTextBit;

    void focusInEvent ( QFocusEvent * e );

signals:
    void stringChanged( const TextBit & );

public slots:
    void textChanged();
    void setId(LingEdit * edit, qlonglong id);
};

#endif // LINGEDIT_H
