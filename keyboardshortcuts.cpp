#include "keyboardshortcuts.h"

#include <QKeyEvent>

KeyboardShortcuts::KeyboardShortcuts()
{

}

KeyboardShortcuts::KeyboardShortcuts(const KeyboardShortcuts &other)
{
    mShortcuts = other.mShortcuts;
}

QKeySequence KeyboardShortcuts::keysequence(const QString & code)
{
    return mShortcuts.value(code, Shortcut("",Qt::Key_unknown)).keysequence;
}

QString KeyboardShortcuts::description(const QString &code)
{
    return mShortcuts.value(code, Shortcut("",Qt::Key_unknown)).description;
}

void KeyboardShortcuts::setKeySequence(const QString &code, const QKeySequence &keystroke)
{
    mShortcuts[code].keysequence = keystroke;
}

void KeyboardShortcuts::setShortcut(const QString &code, const Shortcut &shortcut)
{
    mShortcuts[code] = shortcut;
}

void KeyboardShortcuts::setDefaultShortcuts()
{
    mShortcuts["NextPage"] = Shortcut("Go to next page of text", Qt::Key_PageDown);
    mShortcuts["PreviousPage"] = Shortcut("Go to previous page of text", Qt::Key_PageUp);
    mShortcuts["NormalMouseMode"] = Shortcut("Return to normal mouse mode", Qt::Key_Escape);
    mShortcuts["InsertGlossTextForm"] = Shortcut("Create a new gloss or text form", Qt::Key_Insert);
    mShortcuts["RemoveConstituent"] = Shortcut("Remove syntactic constituent", Qt::Key_Delete);
    mShortcuts["CreateConstituent"] = Shortcut("Create syntactic constituent", Qt::Key_A);
}

QStringList KeyboardShortcuts::codes() const
{
    return mShortcuts.keys();
}

// https://stackoverflow.com/a/10386319/1447002
QKeySequence toKeySequence(const QKeyEvent *event)
{
    return QKeySequence(static_cast<int>(event->key()) | static_cast<int>(event->modifiers()) );
}
