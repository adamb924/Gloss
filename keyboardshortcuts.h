#ifndef KEYBOARDSHORTCUTS_H
#define KEYBOARDSHORTCUTS_H

#include <QHash>
#include <QString>
#include <Qt>

struct Shortcut {
    Shortcut() : description(""), keystroke(Qt::Key_unknown) {}
    Shortcut(const QString & desc, Qt::Key ks) : description(desc), keystroke(ks) {}
    QString description;
    Qt::Key keystroke;
};

class KeyboardShortcuts
{
public:
    KeyboardShortcuts();

    Qt::Key shortcut(const QString & key);
    QString description(const QString & key);
    void setShortcut(const QString & key, Qt::Key keystroke);
    void setDefaultShortcuts();

private:
    QHash<QString,Shortcut> mShortcuts;
};

#endif // KEYBOARDSHORTCUTS_H
