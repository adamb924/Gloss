#include "lingtextedit.h"

#include <QProcess>
#include <QtDebug>
#include <QtWidgets>

LingTextEdit::LingTextEdit(QWidget *parent) :
        QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
//    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateLineNumber()));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    mLineNumber = -1;

    updateLineNumberAreaWidth(0);
}

void LingTextEdit::setWritingSystem(const WritingSystem & ws)
{
    mWritingSystem = ws;
//    setLayoutDirection(mWritingSystem.layoutDirection());
    setStyleSheet(QString("QPlainTextEdit { font-family: %1; font-size: %2pt; }").arg(mWritingSystem.fontFamily()).arg(mWritingSystem.fontSize()));
}

void LingTextEdit::focusInEvent ( QFocusEvent * e )
{
    // http://msdn.microsoft.com/en-us/goglobal/bb896001
    QProcess switchInput;
    switchInput.start( "C:/Program Files/AutoHotkey/AutoHotkey.exe" , QStringList() << mWritingSystem.keyboardCommand() );
    switchInput.waitForFinished();
}

void LingTextEdit::updateLineNumberAreaWidth(int newBlockCount)
{
    if( layoutDirection() == Qt::LeftToRight )
        setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
    else
        setViewportMargins(0, 0, lineNumberAreaWidth(), 0);
}

void LingTextEdit::updateLineNumberArea(const QRect & rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void LingTextEdit::updateLineNumber()
{
    int newLineNumber = textCursor().block().blockNumber();
    if( mLineNumber != newLineNumber )
    {
        mLineNumber = newLineNumber;
        emit lineNumberChanged( mLineNumber );
    }
}

void LingTextEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int LingTextEdit::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void LingTextEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
//    if( layoutDirection() == Qt::LeftToRight )
//        lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
//    else
//        lineNumberArea->setGeometry(QRect(cr.right() - lineNumberAreaWidth(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void LingTextEdit::setLineNumber(int line)
{
    if( line < document()->blockCount() )
    {
        int position = document()->findBlockByLineNumber(line).position();
        QTextCursor cursor = textCursor();
        cursor.setPosition( position, QTextCursor::MoveAnchor );
        centerCursor();
    }
}

void LingTextEdit::highlightCurrentLine()
{    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}
