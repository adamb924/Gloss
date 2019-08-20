#include "keyboardshortcuts.h"

KeyboardShortcuts::KeyboardShortcuts()
{

}

Qt::Key KeyboardShortcuts::shortcut(const QString & key)
{
    return mShortcuts.value(key, Shortcut("",Qt::Key_unknown)).keystroke;
}

QString KeyboardShortcuts::description(const QString &key)
{
    return mShortcuts.value(key, Shortcut("",Qt::Key_unknown)).description;
}

void KeyboardShortcuts::setShortcut(const QString &key, Qt::Key keystroke)
{
    mShortcuts[key].keystroke = keystroke;
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
