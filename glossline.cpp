#include "glossline.h"

#include "writingsystem.h"

GlossLine::GlossLine()
{
}

GlossLine::GlossLine(LineType type, WritingSystem *ws)
{
    mType = type;
    mWritingSystem = ws;
}

void GlossLine::setType(LineType type)
{
    mType = type;
}

void GlossLine::setWritingSystem(WritingSystem *ws)
{
    mWritingSystem = ws;
}

GlossLine::LineType GlossLine::type() const
{
    return mType;
}

WritingSystem* GlossLine::writingSystem() const
{
    return mWritingSystem;
}
