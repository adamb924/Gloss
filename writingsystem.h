#ifndef WRITINGSYSTEM_H
#define WRITINGSYSTEM_H

#include <Qt>
#include <QString>

class WritingSystem
{
public:
    WritingSystem(const qlonglong id, const QString & name, const QString & abbreviation, const QString & flexString, const QString & keyboardCommand, Qt::LayoutDirection layoutDirection, QString fontFamily, int fontSize);
    WritingSystem(const qlonglong id, const QString & abbreviation, const QString & fontFamily, const QString & flexString, Qt::LayoutDirection layoutDirection);

    qlonglong id() const;
    QString name() const;
    QString abbreviation() const;
    QString flexString() const;
    QString keyboardCommand() const;
    Qt::LayoutDirection layoutDirection() const;

    QString summaryString() const;

    QString fontFamily() const;
    int fontSize() const;

private:
    QString mName, mAbbreviation, mFlexString, mKeyboardCommand, mFontFamily;
    qlonglong mId;
    int mFontSize;
    Qt::LayoutDirection mLayoutDirection;
};

#endif // WRITINGSYSTEM_H
