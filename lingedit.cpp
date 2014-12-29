#include "lingedit.h"

#include <QProcess>
#include <QtDebug>
#include <QKeyEvent>
#include <QColor>

#include "textbit.h"

LingEdit::LingEdit(QWidget *parent) :
    QLineEdit(parent), mOverrideFontSize(-1), mTextBit(TextBit()), mSpecialBorder(false)
{
    connect(this,SIGNAL(editingFinished()),this,SLOT(textChanged()));
}

LingEdit::LingEdit(const TextBit & bit, QWidget *parent) :
    QLineEdit(parent), mOverrideFontSize(-1), mTextBit(bit), mSpecialBorder(false)
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
    mTextBit.setText( text() );
    setWritingSystem( bit.writingSystem() );
    if( mTextBit != bit )
    {
        mTextBit = bit;
        setText( bit.text() );
        emit stringChanged(mTextBit, this);
    }
}

void LingEdit::focusInEvent ( QFocusEvent * e )
{
    QLineEdit::focusInEvent(e);

    // http://msdn.microsoft.com/en-us/goglobal/bb896001
    QProcess switchInput;
    switchInput.start( mTextBit.writingSystem().keyboardCommand() );
    switchInput.waitForFinished();

    refreshStyle();
}

void LingEdit::focusOutEvent ( QFocusEvent * e )
{
    QLineEdit::focusOutEvent(e);

    refreshStyle();
}

void LingEdit::keyPressEvent(QKeyEvent *event)
{
    QLineEdit::keyPressEvent(event);
    if( event->key() == Qt::Key_Insert )
    {
        emit insertPressed( mTextBit.writingSystem() );
    }
}

void LingEdit::textChanged()
{
    if( mTextBit.text() != text() )
    {
        mTextBit.setText(text());
        updateGeometry();
        emit stringChanged( mTextBit, this );
    }
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
    setStyleSheet( QString(" QLineEdit { font-family: %1; font-size: %2pt; border: 1px solid %3; %4 }")
                   .arg(mTextBit.writingSystem().fontFamily())
                   .arg( mOverrideFontSize == -1 ? mTextBit.writingSystem().fontSize() : mOverrideFontSize )
                   .arg( hasFocus() ? "#c0c0c0" : "#f0f0f0" )
                   .arg( mSpecialBorder && hasFocus() ? "border-left-color: #0000ff;" : mSpecialBorder ? "border-left-color: #aaaaff;" : "" ) );
}

void LingEdit::setFontSize(int fontSize)
{
    if( mOverrideFontSize != fontSize )
    {
        mOverrideFontSize = fontSize;
        refreshStyle();
    }
}

void LingEdit::setSpecialBorder(bool special)
{
    if( mSpecialBorder != special )
    {
        mSpecialBorder = special;
        refreshStyle();
    }
}

QSize LingEdit::sizeHint() const
{
    QSize sizeHint = QLineEdit::sizeHint();
    sizeHint.setWidth( fontMetrics().width(mTextBit.text()) + 10 );
    return sizeHint;
}

QSize LingEdit::minimumSizeHint() const
{
    QSize sizeHint = QLineEdit::minimumSizeHint();
    sizeHint.setWidth( fontMetrics().width(mTextBit.text()) + 10 );
    return sizeHint;
}
