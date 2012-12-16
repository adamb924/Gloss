/*!
  \class LingTextEdit
  \ingroup GUI
  \brief A QTextEdit subclass. It has an extra signal compared to QLineEdit. It also implements focusInEvent(QFocusEvent*) so that when the widget receives focus, the current writing system is changed.

  This class also uses LineNumberArea, and associated code to connect it to LingTextEdit, modified every so slightly from Qt's Code Editor example code.
*/

#ifndef LINGTEXTEDIT_H
#define LINGTEXTEDIT_H

#include <QPlainTextEdit>

#include "writingsystem.h"

class LingTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit LingTextEdit(QWidget *parent = 0);

    void setWritingSystem(const WritingSystem & ws);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

signals:
    //! \brief Emitted whenever the line number of the cursor is changed. \a line is 0-indexed.
    void lineNumberChanged(int line);

public slots:
    void setLineNumber(int line);
    void highlightCurrentLine();

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &, int);
    void updateLineNumber();

private:
    WritingSystem mWritingSystem;
    QWidget *lineNumberArea;

    int mLineNumber;

    void resizeEvent(QResizeEvent *event);
    void focusInEvent ( QFocusEvent * e );
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(LingTextEdit *editor) : QWidget(editor) {
        mEditor = editor;
    }

    QSize sizeHint() const {
        return QSize(mEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) {
        mEditor->lineNumberAreaPaintEvent(event);
    }

private:
    LingTextEdit *mEditor;
};


#endif // LINGTEXTEDIT_H
