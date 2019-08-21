#include "keystrokedetectinglineedit.h"

#include <QKeyEvent>
#include <QtDebug>

#include "keyboardshortcuts.h"

KeystrokeDetectingLineEdit::KeystrokeDetectingLineEdit(QWidget *parent) :
    QLineEdit(parent)
{

}

void KeystrokeDetectingLineEdit::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void KeystrokeDetectingLineEdit::keyReleaseEvent(QKeyEvent *event)
{
    setText( toKeySequence(event).toString() );
}
