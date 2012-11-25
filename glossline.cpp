#include "glossline.h"

GlossLine::GlossLine()
{
}

GlossLine::GlossLine(Project::GlossLineType type, WritingSystem *ws)
{
    mType = type;
    mWritingSystem = ws;
}

void GlossLine::setType(Project::GlossLineType type)
{
    mType = type;
}

void GlossLine::setWritingSystem(WritingSystem *ws)
{
    mWritingSystem = ws;
}

Project::GlossLineType GlossLine::type() const
{
    return mType;
}

WritingSystem* GlossLine::writingSystem() const
{
    return mWritingSystem;
}
