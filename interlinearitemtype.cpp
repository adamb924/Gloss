#include "interlinearitemtype.h"
#include "writingsystem.h"

InterlinearItemType::InterlinearItemType()
{
}

InterlinearItemType::InterlinearItemType(LineType type, const WritingSystem & ws)
{
    mType = type;
    mWritingSystem = ws;
}

void InterlinearItemType::setType(LineType type)
{
    mType = type;
}

void InterlinearItemType::setWritingSystem(const WritingSystem & ws)
{
    mWritingSystem = ws;
}

InterlinearItemType::LineType InterlinearItemType::type() const
{
    return mType;
}

WritingSystem InterlinearItemType::writingSystem() const
{
    return mWritingSystem;
}