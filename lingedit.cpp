#include "lingedit.h"

#include <QProcess>
#include <QtDebug>

#include "textbit.h"

LingEdit::LingEdit(QWidget *parent) :
    QLineEdit(parent), mOverrideFontSize(-1), mTextBit(TextBit())
{
    connect(this,SIGNAL(editingFinished()),this,SLOT(textChanged()));
}

LingEdit::LingEdit(const TextBit & bit, QWidget *parent) :
    QLineEdit(parent), mOverrideFontSize(-1), mTextBit(bit)
{
    setWritingSystem( mTextBit.writingSystem() );
    setText( mTextBit.text() );
    connect(this,SIGNAL(editingFinished()),this,SLOT(textChanged()));
}

LingEdit::~LingEdit()
{
}

void LingEdit::setTextBit( const TextBit & bit )
{
    if( mTextBit != bit )
    {
        mTextBit = bit;
        setText( bit.text() );
        emit stringChanged(mTextBit, this);
    }
}

void LingEdit::focusInEvent ( QFocusEvent * e )
{
    Q_UNUSED(e);
//    QWidget::focusInEvent(e);

    // http://msdn.microsoft.com/en-us/goglobal/bb896001
    QProcess switchInput;
    switchInput.start( mTextBit.writingSystem().keyboardCommand() );
    switchInput.waitForFinished();

    refreshStyle();
}

//void LingEdit::focusOutEvent ( QFocusEvent * e )
//{
//    QWidget::focusOutEvent(e);

//    refreshStyle();
//}

void LingEdit::textChanged()
{
    if( mTextBit.text() != text() )
    {
        mTextBit.setText(text());
        emit stringChanged( mTextBit, this );
    }
}

void LingEdit::setId(LingEdit * edit, qlonglong id)
{
    //! @todo Rewrite this method of use \a edit?
    Q_UNUSED(edit);
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
    mTextBit.setWritingSystem( ws );

    refreshStyle();
    setLayoutDirection(ws.layoutDirection());
    setToolTip(ws.name());
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

void LingEdit::refreshStyle()
{
    QString borderColor = "#f0f0f0";
//    if( hasFocus() )
//        borderColor = "#0000ff";
    setStyleSheet(QString(" QLineEdit { font-family: %1; font-size: %2pt; border: 1px solid %3; }").arg(mTextBit.writingSystem().fontFamily()).arg( mOverrideFontSize == -1 ? mTextBit.writingSystem().fontSize() : mOverrideFontSize ).arg(borderColor));
}

void LingEdit::setFontSize(int fontSize)
{
    mOverrideFontSize = fontSize;
    refreshStyle();
}
