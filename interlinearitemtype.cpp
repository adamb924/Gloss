#include "interlinearitemtype.h"
#include "writingsystem.h"

InterlinearItemType::InterlinearItemType(LineType type, const WritingSystem & ws)
{
    mType = type;
    mWritingSystem = ws;
}

InterlinearItemType::InterlinearItemType(const QString & type, const WritingSystem & ws)
{
    if( type == "text ")
    {
        mType = InterlinearItemType::Text;
    }
    else if( type == "gloss" )
    {
        mType = InterlinearItemType::Gloss;
    }
    else if( type == "analysis" )
    {
        mType = InterlinearItemType::Analysis;
    }
    else
    {
        mType = InterlinearItemType::Text;
    }
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
