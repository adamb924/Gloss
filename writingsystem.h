/*!
  \class WritingSystem
  \ingroup DataStructures
  \brief A data class holding data about a writing system.

  The corresponding SQL table is WritingSystems.

  Note that the equality operator for this class is defined so that two WritingSystem objects are identical if they have the same mFlexString value. Similarly, a WritingSystem object can be compared directly to a QString that has a flex string.
*/

#ifndef WRITINGSYSTEM_H
#define WRITINGSYSTEM_H

#include <Qt>
#include <QString>
#include <QHash>

class WritingSystem
{
public:
    WritingSystem();
    WritingSystem(const qlonglong id, const QString & name, const QString & abbreviation, const QString & flexString, const QString & keyboardCommand, Qt::LayoutDirection layoutDirection, QString fontFamily, int fontSize);

    qlonglong id() const;
    QString name() const;
    QString abbreviation() const;
    QString flexString() const;
    QString keyboardCommand() const;
    Qt::LayoutDirection layoutDirection() const;

    QString summaryString() const;

    bool operator==(const WritingSystem & other) const;
    bool operator!=(const WritingSystem & other) const;
    bool operator==(const QString & flexString) const;
    WritingSystem& operator=(const WritingSystem & other);

    QString fontFamily() const;
    int fontSize() const;

    bool isNull() const;
    bool isValid() const;

private:
    qlonglong mId;
    QString mName, mAbbreviation, mFlexString, mKeyboardCommand, mFontFamily;
    int mFontSize;
    Qt::LayoutDirection mLayoutDirection;
};


inline uint qHash(const WritingSystem & key)
{
    return qHash(key.flexString());
}

#endif // WRITINGSYSTEM_H
