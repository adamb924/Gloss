#include "textbit.h"
#include "writingsystem.h"

TextBit::TextBit()
{
}

TextBit::TextBit(const QString & text, const WritingSystem & ws, qlonglong id )
{
    mText = text;
    mWritingSystem = ws;
    mId = id;
}

TextBit::TextBit(const TextBit & other)
{
    mText = other.mText;
    mWritingSystem = other.mWritingSystem;
    mId = other.mId;
}

TextBit& TextBit::operator=(const TextBit & other)
                           {
    mText = other.mText;
    mWritingSystem = other.mWritingSystem;
    mId = other.mId;
    return *this;
}

bool TextBit::operator==(const TextBit & other) const
{
    return mText == other.mText && mWritingSystem == other.mWritingSystem && mId == other.mId;
}

void TextBit::setText(const QString & text)
{
    mText = text;
}

void TextBit::setWritingSystem(const WritingSystem & ws)
{
    mWritingSystem = ws;
}

QString TextBit::text() const
{
    return mText;
}

WritingSystem TextBit::writingSystem() const
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
