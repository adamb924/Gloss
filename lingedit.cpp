#include "lingedit.h"

#include <QProcess>
#include <QtDebug>

#include "textbit.h"

LingEdit::LingEdit(QWidget *parent) :
    QLineEdit(parent)
{
    mTextBit = TextBit();
    connect(this,SIGNAL(editingFinished()),this,SLOT(textChanged()));
}

LingEdit::LingEdit(const TextBit & bit, QWidget *parent) :
    QLineEdit(parent)
{
    mTextBit = bit;
    setWritingSystem( mTextBit.writingSystem() );
    setText( mTextBit.text() );
    connect(this,SIGNAL(editingFinished()),this,SLOT(textChanged()));
}

void LingEdit::setTextBit( const TextBit & bit )
{
    if( mTextBit != bit )
    {
        mTextBit = bit;
        setText( bit.text() );
        emit stringChanged(mTextBit);
    }
}

void LingEdit::focusInEvent ( QFocusEvent * e )
{
    // http://msdn.microsoft.com/en-us/goglobal/bb896001
    QProcess switchInput;
    switchInput.start( "C:/Program Files/AutoHotkey/AutoHotkey.exe" , QStringList() << mTextBit.writingSystem().keyboardCommand() );
    switchInput.waitForFinished();
}

void LingEdit::textChanged()
{
    if( mTextBit.text() != text() )
    {
        mTextBit.setText(text());
        emit stringChanged( mTextBit );
    }
}

void LingEdit::setId(LingEdit * edit, qlonglong id)
{
    mTextBit.setId(id);
}

qlonglong LingEdit::id() const
{
    return mTextBit.id();
}

void LingEdit::matchTextAlignmentTo( Qt::LayoutDirection target )
{
    if( target == Qt::LeftToRight )
        setAlignment( mTextBit.writingSystem().layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight );
    else
        setAlignment( mTextBit.writingSystem().layoutDirection() == Qt::LeftToRight ? Qt::AlignRight : Qt::AlignLeft );
}

void LingEdit::setWritingSystem( const WritingSystem & ws )
{
    setStyleSheet(QString(" QLineEdit { font-family: %1; font-size: %2pt; border: 1px solid #f0f0f0; }").arg(ws.fontFamily()).arg(ws.fontSize()));
    setLayoutDirection(ws.layoutDirection());
    setToolTip(ws.name());
    mTextBit.setWritingSystem( ws );
}

TextBit LingEdit::textBit() const
{
    return TextBit( text(), mTextBit.writingSystem(), mTextBit.id() );
}

void LingEdit::updateMatchingTextBit( const TextBit & bit )
{
    if( bit.id() == mTextBit.id() && bit.writingSystem() == mTextBit.writingSystem() )
        setTextBit( bit );
}
