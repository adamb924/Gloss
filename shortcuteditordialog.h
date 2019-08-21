#ifndef SHORTCUTEDITORDIALOG_H
#define SHORTCUTEDITORDIALOG_H

#include <QDialog>

#include "keyboardshortcuts.h"

class QStringListModel;

namespace Ui {
class ShortcutEditorDialog;
}

class ShortcutEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShortcutEditorDialog(KeyboardShortcuts shortcuts, QWidget *parent = nullptr);
    ~ShortcutEditorDialog();

    KeyboardShortcuts shortcuts() const;

private slots:
    void itemChanged(const QModelIndex &current, const QModelIndex &previous);
    void setKeystroke(const QString &text);
    void initialize();
    void restoreDefaults();

private:
    Ui::ShortcutEditorDialog *ui;
    KeyboardShortcuts mShortcuts;

    QStringListModel * mModel;
    QString mCurrentCode;
};

#endif // SHORTCUTEDITORDIALOG_H
