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

class TextBit : public QObject
{
    Q_OBJECT
public:
    TextBit();
    TextBit(const QString & text, const WritingSystem & ws, qlonglong id = -1 );
    TextBit(const TextBit & other);

    bool operator==(const TextBit & other) const;
    TextBit& operator=(const TextBit & other);
    bool operator!=(const TextBit & other) const;

    QString text() const;
    WritingSystem writingSystem() const;
    qlonglong id() const;

public slots:
    void setText(const QString & text);
    void setWritingSystem(const WritingSystem & ws);
    void setId(qlonglong id);

private:
    QString mText;
    WritingSystem mWritingSystem;
    // user defined; not guaranteed to be set
    qlonglong mId;
};

inline uint qHash(const TextBit & key)
{
    return qHash(key.text() + key.id() + key.writingSystem().flexString() );
}

typedef QHash<WritingSystem, TextBit> TextBitHash;
typedef QHashIterator<WritingSystem, TextBit> TextBitHashIterator;
typedef QMutableHashIterator<WritingSystem, TextBit> TextBitMutableHashIterator;

#endif // TEXTBIT_H
