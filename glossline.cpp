#include "glossline.h"
#include "writingsystem.h"

GlossLine::GlossLine()
{
}

GlossLine::GlossLine(LineType type, const WritingSystem & ws)
{
    mType = type;
    mWritingSystem = ws;
}

void GlossLine::setType(LineType type)
{
    mType = type;
}

void GlossLine::setWritingSystem(const WritingSystem & ws)
{
    mWritingSystem = ws;
}

GlossLine::LineType GlossLine::type() const
{
    return mType;
}

WritingSystem GlossLine::writingSystem() const
{
    return mWritingSystem;
}
