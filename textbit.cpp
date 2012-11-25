#include "textbit.h"

TextBit::TextBit()
{
}

TextBit::TextBit(const QString & text, WritingSystem *ws, qlonglong id )
{
    mText = text;
    mWritingSystem = ws;
    mId = id;
}

TextBit::TextBit(TextBit const & other)
{
    mText = other.mText;
    mWritingSystem = other.mWritingSystem;
    mId = other.mId;
}


void TextBit::setText(const QString & text)
{
    mText = text;
}

void TextBit::setWritingSystem(WritingSystem *ws)
{
    mWritingSystem = ws;
}

QString TextBit::text() const
{
    return mText;
}

WritingSystem* TextBit::writingSystem() const
{
    return mWritingSystem;
}

void TextBit::setId(qlonglong id)
{
    mId = id;
}

qlonglong TextBit::id() const
{
    return mId;
}
