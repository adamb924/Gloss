#include "lingtextedit.h"

#include <QProcess>
#include <QtDebug>

LingTextEdit::LingTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
}

void LingTextEdit::setWritingSystem(const WritingSystem & ws)
{
    mWritingSystem = ws;
    setStyleSheet(QString("font-family: %1; font-size: %2pt;").arg(mWritingSystem.fontFamily()).arg(mWritingSystem.fontSize()));
    setLayoutDirection(ws.layoutDirection());
}

void LingTextEdit::focusInEvent ( QFocusEvent * e )
{
    // http://msdn.microsoft.com/en-us/goglobal/bb896001
    QProcess switchInput;
    switchInput.start( "C:/Program Files/AutoHotkey/AutoHotkey.exe" , QStringList() << mWritingSystem.keyboardCommand() );
    switchInput.waitForFinished();
}
