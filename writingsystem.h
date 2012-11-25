#ifndef WRITINGSYSTEM_H
#define WRITINGSYSTEM_H

#include <Qt>
#include <QString>

class WritingSystem
{
public:
    WritingSystem();
    WritingSystem(const QString & name, const QString & abbreviation, const QString & flexString, const QString & keyboardCommand, Qt::LayoutDirection layoutDirection);

    QString name() const;
    QString abbreviation() const;
    QString flexString() const;
    QString keyboardCommand() const;
    Qt::LayoutDirection layoutDirection() const;

private:
    QString mName, mAbbreviation, mFlexString, mKeyboardCommand;
    Qt::LayoutDirection mLayoutDirection;
};

#endif // WRITINGSYSTEM_H
