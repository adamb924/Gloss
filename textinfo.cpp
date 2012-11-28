#include "textinfo.h"

TextInfo::TextInfo()
{
}

TextInfo::TextInfo(const QString & name, Project::BaselineMode bm, WritingSystem *ws)
{
    mName = name;
    mBaselineMode = bm;
    mBaselineWritingSystem = ws;
}

QString TextInfo::name() const
{
    return mName;
}

void TextInfo::setName(const QString & name)
{
    mName = name;
}

Project::BaselineMode TextInfo::baselineMode() const
{
    return mBaselineMode;
}
void TextInfo::setBaselineMode(Project::BaselineMode bm)
{
    mBaselineMode = bm;
}

WritingSystem* TextInfo::writingSystem() const
{
    return mBaselineWritingSystem;
}

void TextInfo::setWritingSystem(WritingSystem *ws)
{
    mBaselineWritingSystem = ws;
}
