#include "lingtextedit.h"

#include <QProcess>
#include <QtDebug>

LingTextEdit::LingTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
}

void LingTextEdit::setWritingSystem(const WritingSystem & ws)
{
    mWritingSystem = ws;
    setStyleSheet(QString("font-family: %1; font-size: %2pt;").arg(mWritingSystem.fontFamily()).arg(mWritingSystem.fontSize()));
}

void LingTextEdit::focusInEvent ( QFocusEvent * e )
{
    // http://msdn.microsoft.com/en-us/goglobal/bb896001
    QProcess *switchInput = new QProcess();
    switchInput->start( "C:/Program Files/AutoHotkey/AutoHotkey.exe" , QStringList() << mWritingSystem.keyboardCommand() );
}
