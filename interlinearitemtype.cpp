#include "interlinearitemtype.h"
#include "writingsystem.h"

InterlinearItemType::InterlinearItemType()
{
    mType = Null;
}

InterlinearItemType::InterlinearItemType(LineType type, const WritingSystem & ws)
{
    mType = type;
    mWritingSystem = ws;
}

InterlinearItemType::InterlinearItemType(const QString & type, const WritingSystem & ws)
{
    QString string = type.toLower();
    if( string == "text")
    {
        mType = InterlinearItemType::Text;
    }
    else if( string == "gloss" )
    {
        mType = InterlinearItemType::Gloss;
    }
    else if( string == "analysis" )
    {
        mType = InterlinearItemType::Analysis;
    }
    else
    {
        mType = InterlinearItemType::Immutable;
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
