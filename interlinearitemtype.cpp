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
    else if( string == "analysis" )
    {
        mType = InterlinearItemType::Analysis;
    }
    else if( string == "immutable-text" )
    {
        mType = InterlinearItemType::ImmutableText;
    }
    else if( string == "immutable-gloss" )
    {
        mType = InterlinearItemType::ImmutableGloss;
    }
    else
    {
        mType = InterlinearItemType::Null;
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

QDebug operator<<(QDebug dbg, const InterlinearItemType &type)
{
    switch(type.type())
    {
    case InterlinearItemType::ImmutableText:
        dbg.nospace() << "InterlinearItemType(ImmutableText)";
        break;
    case InterlinearItemType::ImmutableGloss:
        dbg.nospace() << "InterlinearItemType(ImmutableGloss)";
        break;
    case InterlinearItemType::Text:
        dbg.nospace() << "InterlinearItemType(Text)";
        break;
    case InterlinearItemType::Gloss:
        dbg.nospace() << "InterlinearItemType(Gloss)";
        break;
    case InterlinearItemType::Analysis:
        dbg.nospace() << "InterlinearItemType(Analysis)";
        break;
    case InterlinearItemType::Null:
    default:
        dbg.nospace() << "InterlinearItemType(Null)";
        break;
    };
    return dbg.maybeSpace();
}
