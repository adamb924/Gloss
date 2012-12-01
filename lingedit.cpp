#include "lingedit.h"

#include <QProcess>
#include <QtDebug>

#include "textbit.h"

LingEdit::LingEdit(const TextBit & bit, QWidget *parent) :
    QLineEdit(parent)
{
    mTextBit = bit;

//    setStyleSheet("border: 1px solid gray");

    this->setLayoutDirection(bit.writingSystem()->layoutDirection());

    connect(this,SIGNAL(editingFinished()),this,SLOT(textChanged()));
}

void LingEdit::focusInEvent ( QFocusEvent * e )
{
//    QProcess *switchInput = new QProcess();
//    switchInput->start( "C:/Program Files/AutoHotkey/AutoHotkey.exe" , QStringList() << mWritingSystem.keyboardCommand() );
}

void LingEdit::textChanged()
{
    mTextBit.setText(text());
    emit stringChanged( mTextBit );
}
