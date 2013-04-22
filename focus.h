#ifndef FOCUS_H
#define FOCUS_H

#include <Qt>

class Focus
{
public:
    enum Type { Interpretation = Qt::UserRole, TextForm, Gloss, GlossItem, Null };
    enum ListType { TypeList = Qt::UserRole + 1, IndexList = Qt::UserRole + 2 };

    Focus();
    Focus(Type t, qlonglong i);

    Focus::Type type() const;
    qlonglong index() const;

private:
    Type mType;
    qlonglong mIndex;
};

#endif // FOCUS_H
