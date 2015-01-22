/*!
  \class Focus
  \ingroup Data
  \ingroup Interlinear
  \brief This class represents focused items in an interlinear text display. In displaying a line of interlinear text following a search, for instance, it can be helpful for a particular object to be focused (i.e., highlighted visually) for the user.
*/

#ifndef FOCUS_H
#define FOCUS_H

#include <Qt>
#include <QString>

class Focus
{
public:
    enum Type { Interpretation = Qt::UserRole, TextForm, Gloss, GlossItem, WholeStringSearch, SubStringSearch, Null };
    enum ListType { TypeList = Qt::UserRole + 1, IndexList = Qt::UserRole + 2 };

    Focus();
    Focus(Type t, qlonglong i);
    Focus(Type t, const QString & bit);

    //! \brief Return the type (Focus::Type) of the object.
    Focus::Type type() const;

    //! \brief Return the index that is being focused. The interpretation of index() will depend on type().
    qlonglong index() const;

    //! \brief Return a string representation of the type (Focus::Type) of the object.
    QString typeString() const;

    QString searchString() const;

private:
    Type mType;
    qlonglong mIndex;
    QString mString;
};

QDebug operator<<(QDebug dbg, const Focus &key);

#endif // FOCUS_H
