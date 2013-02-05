#include "focus.h"

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
