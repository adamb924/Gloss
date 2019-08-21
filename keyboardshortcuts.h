#ifndef KEYBOARDSHORTCUTS_H
#define KEYBOARDSHORTCUTS_H

#include <QHash>
#include <QString>
#include <QKeySequence>

class QKeyEvent;

struct Shortcut {
    Shortcut() : description(""), keysequence(Qt::Key_unknown) {}
    Shortcut(const QString & desc, Qt::Key ks) : description(desc), keysequence(ks) {}
    QString description;
    QKeySequence keysequence;
};

class KeyboardShortcuts
{
public:
    KeyboardShortcuts();
    KeyboardShortcuts(const KeyboardShortcuts & other);

    QKeySequence keysequence(const QString & code);
    QString description(const QString & code);
    void setKeySequence(const QString & code, const QKeySequence & keystroke);
    void setShortcut(const QString & code, const Shortcut & keysequence);
    void setDefaultShortcuts();
    QStringList codes() const;

private:
    QHash<QString,Shortcut> mShortcuts;
};

QKeySequence toKeySequence(const QKeyEvent *event);


#endif // KEYBOARDSHORTCUTS_H
