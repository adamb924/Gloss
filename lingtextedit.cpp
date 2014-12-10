#include "lingtextedit.h"

#include <QProcess>

LingTextEdit::LingTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
}

LingTextEdit::LingTextEdit(const TextBit &bit, QWidget *parent) : QTextEdit(parent)
{
    setWritingSystem( bit.writingSystem() );
    setText( bit.text() );
}

TextBit LingTextEdit::textBit() const
{
    return TextBit( toPlainText() , mWritingSystem );
}

void LingTextEdit::setTextBit(const TextBit &bit)
{
    setWritingSystem( bit.writingSystem() );
    setText( bit.text() );
}

void LingTextEdit::setWritingSystem(const WritingSystem &ws)
{
    if( mWritingSystem != ws )
    {
        mWritingSystem = ws;
        refreshStyle();
        setLayoutDirection(ws.layoutDirection());
        setToolTip(ws.name());
    }
}

void LingTextEdit::refreshStyle()
{
    if( mWritingSystem.isNull() ) return;
    setStyleSheet( QString(" QTextEdit { font-family: %1; font-size: %2pt; border: 1px solid #000; }")
                   .arg(mWritingSystem.fontFamily()).arg( mWritingSystem.fontSize() ) );
}

void LingTextEdit::focusInEvent(QFocusEvent *e)
{
    QTextEdit::focusInEvent(e);

    // http://msdn.microsoft.com/en-us/goglobal/bb896001
    QProcess switchInput;
    switchInput.start( mWritingSystem.keyboardCommand() );
    switchInput.waitForFinished();
}
