#include "text.h"

Text::Text()
{
}


Text::Text(const QString & name, Project::BaselineMode bm, WritingSystem *ws)
{
    mName = name;
    mBaselineMode = bm;
    mBaselineWritingSystem = ws;
}

QString Text::name() const
{
    return mName;
}

void Text::setName(const QString & name)
{
    mName = name;
}

Project::BaselineMode Text::baselineMode() const
{
    return mBaselineMode;
}
void Text::setBaselineMode(Project::BaselineMode bm)
{
    mBaselineMode = bm;
}

WritingSystem* Text::writingSystem() const
{
    return mBaselineWritingSystem;
}

void Text::setWritingSystem(WritingSystem *ws)
{
    mBaselineWritingSystem = ws;
}

QString Text::baselineText() const
{

}

void Text::setBaselineText(const QString & text)
{

}

