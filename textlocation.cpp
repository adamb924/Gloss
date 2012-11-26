#include "textlocation.h"

TextLocation::TextLocation()
{
}

TextLocation::TextLocation(long line, long position)
{
    mLine = line;
    mPosition = position;
}

void TextLocation::setLine(long l)
{
    mLine = l;
}

void TextLocation::setPosition(long p)
{
    mPosition = p;
}

long TextLocation::line() const
{
    return mLine;
}

long TextLocation::position() const
{
    return mPosition;
}
