/*!
  \class TextBit
  \ingroup DataStructures
  \brief A data class for holding a text string, its associated WritingSystem, and an id value. The id value is not guaranteed to be set.

  TextBit is the data container class for most text strings. In places where it makes sense to index a container by WritingSystem, then the type QHash<WritingSystem, QString> is used; this is typedef'd to TextBitHash for short. A TextBit can of course be generated easily from any key/value pair.
*/

#ifndef TEXTBIT_H
#define TEXTBIT_H

#include <QString>
#include "writingsystem.h"

typedef QHash<WritingSystem, QString> TextBitHash;
typedef QHashIterator<WritingSystem, QString> TextBitHashIterator;

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
