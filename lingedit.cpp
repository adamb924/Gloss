#include "lingedit.h"

#include <QProcess>
#include <QtDebug>

#include "textbit.h"

LingEdit::LingEdit(const TextBit & bit, QWidget *parent) :
    QLineEdit(parent)
{
    mTextBit = bit;

    setStyleSheet("border: 1px solid gray");
    setStyleSheet(QString("font-family: %1; font-size: %2pt;").arg(bit.writingSystem().fontFamily()).arg(bit.writingSystem().fontSize()));
    setLayoutDirection(bit.writingSystem().layoutDirection());
    setAlignment(Qt::AlignCenter);

    connect(this,SIGNAL(editingFinished()),this,SLOT(textChanged()));
}

void LingEdit::focusInEvent ( QFocusEvent * e )
{
    // http://msdn.microsoft.com/en-us/goglobal/bb896001
    QProcess *switchInput = new QProcess();
    switchInput->start( "C:/Program Files/AutoHotkey/AutoHotkey.exe" , QStringList() << mTextBit.writingSystem().keyboardCommand() );
}

void LingEdit::textChanged()
{
    mTextBit.setText(text());
    emit stringChanged( mTextBit );
}

void LingEdit::changeId(qlonglong id)
{
    mTextBit.setId(id);
}
