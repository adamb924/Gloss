#include "focus.h"

#include <QString>
#include <QDebug>

Focus::Focus()
{
    mType = Focus::Null;
    mIndex = -1;
}

Focus::Focus(Type t, qlonglong i)
{
    mType = t;
    mIndex = i;
}

Focus::Type Focus::type() const
{
    return mType;
}

qlonglong Focus::index() const
{
    return mIndex;
}

QString Focus::typeString() const
{
    switch( mType )
    {
    case Focus::Interpretation:
        return "Interpretation";
        break;
    case Focus::TextForm:
        return "TextForm";
        break;
    case Focus::Gloss:
        return "Gloss";
        break;
    case Focus::GlossItem:
        return "GlossItem";
        break;
    case Focus::Null:
    default:
        return "Null";
        break;
    }
}

QDebug operator<<(QDebug dbg, const Focus & key)
{
    dbg.nospace() << "Focus( " << key.typeString() << ", Index: " << key.index() << ")";
    return dbg.maybeSpace();
}
