/*!
  \class Focus
  \ingroup Data
  \ingroup Interlinear
  \brief This class represents focused items in an interlinear text display. In displaying a line of interlinear text following a search, for instance, it can be helpful for a particular object to be focused (i.e., highlighted visually) for the user.
*/

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

    //! \brief Return the type (Focus::Type) of the object.
    Focus::Type type() const;

    //! \brief Return the index that is being focused. The interpretation of index() will depend on type().
    qlonglong index() const;

private:
    Type mType;
    qlonglong mIndex;
};

#endif // FOCUS_H
