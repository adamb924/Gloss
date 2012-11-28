#include "writingsystem.h"

WritingSystem::WritingSystem()
{
}

WritingSystem::WritingSystem(const QString & name, const QString & abbreviation, const QString & flexString, const QString & keyboardCommand, Qt::LayoutDirection layoutDirection, QString fontFamily, int fontSize)
{
    mName = name;
    mAbbreviation = abbreviation;
    mFlexString = flexString;
    mKeyboardCommand = keyboardCommand;
    mLayoutDirection = layoutDirection;
    mFontFamily = fontFamily;
    mFontSize = fontSize;
}

WritingSystem::WritingSystem(const QString & abbreviation, const QString & fontFamily, const QString & flexString, Qt::LayoutDirection layoutDirection)
{
    mAbbreviation = abbreviation;
    mFontFamily = fontFamily;
    mFlexString = flexString;
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

QString WritingSystem::fontFamily() const
{
    return mFontFamily;
}

int WritingSystem::fontSize() const
{
    return mFontSize;
}

QString WritingSystem::summaryString() const
{
    return QString("%1 (%2, %3)").arg(mName).arg(mAbbreviation).arg(mFlexString);
}
