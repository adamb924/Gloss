/*!
  \class TextBit
  \ingroup Data
  \brief A data class for holding a text string, its associated WritingSystem, and an id value. The id value is not guaranteed to be set. (It may be extraneous, as of 12/4/2012.)
*/

#ifndef TEXTBIT_H
#define TEXTBIT_H

#include <QString>
#include "writingsystem.h"

class TextBit
{
public:
    TextBit();
    TextBit(const QString & text, const WritingSystem &, qlonglong id = -1 );
    TextBit(TextBit const & other);

    void setText(const QString & text);
    QString text() const;

    void setWritingSystem(const WritingSystem & ws);
    WritingSystem writingSystem() const;

    void setId(qlonglong id);
    qlonglong id() const;

private:
    QString mText;
    WritingSystem mWritingSystem;
    // user defined; not guaranteed to be set
    qlonglong mId;
};

#endif // TEXTBIT_H
