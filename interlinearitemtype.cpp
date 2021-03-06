#include "interlinearitemtype.h"

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
    else if( string == "immutable-text" )
    {
        mType = InterlinearItemType::ImmutableText;
    }
    else if( string == "immutable-gloss" )
    {
        mType = InterlinearItemType::ImmutableGloss;
    }
    else if( string == "pos-tagging" )
    {
        mType = InterlinearItemType::PosTagging;
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

QString InterlinearItemType::typeString() const
{
    switch(mType)
    {
    case InterlinearItemType::ImmutableText:
        return QObject::tr("Immutable Text");
    case InterlinearItemType::ImmutableGloss:
        return QObject::tr("Immutable Gloss");
    case InterlinearItemType::Text:
        return QObject::tr("Text Form");
    case InterlinearItemType::Gloss:
        return QObject::tr("Gloss");
    case InterlinearItemType::Analysis:
        return QObject::tr("Analysis");
    case InterlinearItemType::PosTagging:
        return QObject::tr("Part of Speech Tagging");
    case InterlinearItemType::Null:
        return QObject::tr("(Null)");
    }
    return QObject::tr("(Null)");
}

QString InterlinearItemType::typeXmlString() const
{
    switch(mType)
    {
    case InterlinearItemType::ImmutableText:
        return QObject::tr("immutable-text");
    case InterlinearItemType::ImmutableGloss:
        return QObject::tr("immutable-gloss");
    case InterlinearItemType::Text:
        return QObject::tr("text");
    case InterlinearItemType::Gloss:
        return QObject::tr("gloss");
    case InterlinearItemType::Analysis:
        return QObject::tr("analysis");
    case InterlinearItemType::PosTagging:
        return QObject::tr("pos-tagging");
    case InterlinearItemType::Null:
        return QObject::tr("(Null)");
    }
    return QObject::tr("(Null)");
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
    case InterlinearItemType::PosTagging:
        dbg.nospace() << "InterlinearItemType(PosTagging)";
        break;
    case InterlinearItemType::Null:
        dbg.nospace() << "InterlinearItemType(Null)";
        break;
    };
    return dbg.maybeSpace();
}
