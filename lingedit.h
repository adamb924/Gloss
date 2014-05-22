/*!
  \class LingEdit
  \ingroup GUI
  \brief A QLineEdit subclass. It has an extra signal compared to QLineEdit. It also implements focusInEvent(QFocusEvent*) so that when the widget receives focus, the current writing system is changed.
*/

#ifndef LINGEDIT_H
#define LINGEDIT_H

#include <QLineEdit>

#include "writingsystem.h"
#include "textbit.h"

class LingEdit : public QLineEdit
{
    Q_OBJECT
public:

    explicit LingEdit(QWidget *parent = 0);
    LingEdit(const TextBit & bit, QWidget *parent = 0);
    ~LingEdit();

    qlonglong id() const;

    TextBit textBit() const;

    void matchTextAlignmentTo( Qt::LayoutDirection target );

    void setFontSize(int fontSize);

private:
    TextBit mTextBit;

    void focusInEvent ( QFocusEvent * e );
//    void focusOutEvent ( QFocusEvent * e );

    int mOverrideFontSize;

    void refreshStyle();

signals:
    void stringChanged( const TextBit & , LingEdit *);

public slots:
    void textChanged();
    void setId(LingEdit * edit, qlonglong id);
    void setTextBit( const TextBit & bit );
    void setWritingSystem( const WritingSystem & ws );

    //! \brief Sets the text of the editor to the text in \a bit, if the current id and writing system match those in \bit.
    void updateMatchingTextBit( const TextBit & bit );

};

#endif // LINGEDIT_H
