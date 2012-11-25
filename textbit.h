#ifndef TEXTBIT_H
#define TEXTBIT_H

#include <QString>
#include "writingsystem.h"

class TextBit
{
public:
    TextBit();
    TextBit(const QString & text, WritingSystem *ws, qlonglong id = -1 );
    TextBit(TextBit const & other);

    void setText(const QString & text);
    QString text() const;

    void setWritingSystem(WritingSystem *ws);
    WritingSystem* writingSystem() const;

    void setId(qlonglong id);
    qlonglong id() const;

private:
    QString mText;
    WritingSystem *mWritingSystem;
    // user defined; not guaranteed to be set
    qlonglong mId;
};

#endif // TEXTBIT_H
