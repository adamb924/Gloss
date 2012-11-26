#ifndef TEXTLOCATION_H
#define TEXTLOCATION_H

#include <Qt>

class TextLocation
{
public:
    TextLocation();
    TextLocation(long line, long position);
    void setLine(long l);
    void setPosition(long p);
    long line() const;
    long position() const;
private:
    long mLine, mPosition;
};

#endif // TEXTLOCATION_H
