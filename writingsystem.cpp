#include "writingsystem.h"

WritingSystem::WritingSystem()
{
}

WritingSystem::WritingSystem(const QString & name, const QString & abbreviation, const QString & flexString, const QString & keyboardCommand, Qt::LayoutDirection layoutDirection)
{
    mName = name;
    mAbbreviation = abbreviation;
    mFlexString = flexString;
    mKeyboardCommand = keyboardCommand;
    mLayoutDirection = layoutDirection;
}

QString WritingSystem::keyboardCommand() const
{
    return mKeyboardCommand;
}

QString WritingSystem::name() const
{
    return mName;
}

QString WritingSystem::abbreviation() const
{
    return mAbbreviation;
}

QString WritingSystem::flexString() const
{
    return mFlexString;
}

Qt::LayoutDirection WritingSystem::layoutDirection() const
{
    return mLayoutDirection;
}
