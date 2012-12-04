/*!
  \class WritingSystem
  \ingroup Data
  \brief A data class holding data about a writing system.

  The corresponding SQL table is WritingSystems.

  Note that the equality operator for this class is defined so that two WritingSystem objects are identical if they have the same mFlexString value.
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
    WritingSystem(const WritingSystem & other);
    WritingSystem(const qlonglong id, const QString & name, const QString & abbreviation, const QString & flexString, const QString & keyboardCommand, Qt::LayoutDirection layoutDirection, QString fontFamily, int fontSize);
    WritingSystem(const qlonglong id, const QString & abbreviation, const QString & fontFamily, const QString & flexString, Qt::LayoutDirection layoutDirection);

    qlonglong id() const;
    QString name() const;
    QString abbreviation() const;
    QString flexString() const;
    QString keyboardCommand() const;
    Qt::LayoutDirection layoutDirection() const;

    QString summaryString() const;

    bool operator==(const WritingSystem & other) const;
    WritingSystem& operator=(const WritingSystem & other);

    QString fontFamily() const;
    int fontSize() const;

private:
    QString mName, mAbbreviation, mFlexString, mKeyboardCommand, mFontFamily;
    qlonglong mId;
    int mFontSize;
    Qt::LayoutDirection mLayoutDirection;
};


inline uint qHash(const WritingSystem & key)
{
    return qHash(key.flexString());
}

#endif // WRITINGSYSTEM_H
