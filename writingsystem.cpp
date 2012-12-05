#include "writingsystem.h"

WritingSystem::WritingSystem()
{
    mId = 0;
    mName = "";
    mAbbreviation = "";
    mFlexString = "";
    mKeyboardCommand = "";
    mLayoutDirection = Qt::LeftToRight;
    mFontFamily = "";
    mFontSize = 0;
}

WritingSystem::WritingSystem(const WritingSystem & other)
{
    mId = other.mId;
    mName = other.mName;
    mAbbreviation = other.mAbbreviation;
    mFlexString = other.mFlexString;
    mKeyboardCommand = other.mKeyboardCommand;
    mLayoutDirection = other.mLayoutDirection;
    mFontFamily = other.mFontFamily;
    mFontSize = other.mFontSize;
}

WritingSystem::WritingSystem(const qlonglong id, const QString & name, const QString & abbreviation, const QString & flexString, const QString & keyboardCommand, Qt::LayoutDirection layoutDirection, QString fontFamily, int fontSize)
{
    mId = id;
    mName = name;
    mAbbreviation = abbreviation;
    mFlexString = flexString;
    mKeyboardCommand = keyboardCommand;
    mLayoutDirection = layoutDirection;
    mFontFamily = fontFamily;
    mFontSize = fontSize;
}

WritingSystem::WritingSystem(const qlonglong id, const QString & abbreviation, const QString & fontFamily, const QString & flexString, Qt::LayoutDirection layoutDirection)
{
    mId = id;
    mAbbreviation = abbreviation;
    mFontFamily = fontFamily;
    mFlexString = flexString;
    mLayoutDirection = layoutDirection;
}

QString WritingSystem::keyboardCommand() const
{
    return mKeyboardCommand;
}

qlonglong WritingSystem::id() const
{
    return mId;
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

bool WritingSystem::operator==(const WritingSystem & other) const
{
    return mFlexString == other.flexString();
}

bool WritingSystem::operator==(const QString & flexString) const
{
    return mFlexString == flexString;
}

WritingSystem& WritingSystem::operator=(const WritingSystem & other)
{
    mId = other.mId;
    mName = other.mName;
    mAbbreviation = other.mAbbreviation;
    mFlexString = other.mFlexString;
    mKeyboardCommand = other.mKeyboardCommand;
    mLayoutDirection = other.mLayoutDirection;
    mFontFamily = other.mFontFamily;
    mFontSize = other.mFontSize;
    return *this;
}
