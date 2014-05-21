#include "writingsystem.h"

WritingSystem::WritingSystem() : mId(-1), mName(""), mAbbreviation(""), mFlexString(""), mKeyboardCommand(""), mFontFamily(""), mFontSize(0), mLayoutDirection(Qt::LeftToRight)
{
}

WritingSystem::WritingSystem(const qlonglong id, const QString & name, const QString & abbreviation, const QString & flexString, const QString & keyboardCommand, Qt::LayoutDirection layoutDirection, QString fontFamily, int fontSize)
     : mId(id), mName(name), mAbbreviation(abbreviation), mFlexString(flexString), mKeyboardCommand(keyboardCommand), mFontFamily(fontFamily), mFontSize(fontSize), mLayoutDirection(layoutDirection)
{
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
    return mId == other.mId;
}

bool WritingSystem::operator!=(const WritingSystem & other) const
{
    return mFlexString != other.mFlexString;
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

bool WritingSystem::isNull() const
{
    return mId == -1;
}

bool WritingSystem::isValid() const
{
    return mId != -1;
}

uint qHash(const WritingSystem & key)
{
    return qHash(key.flexString());
}

QDebug operator<<(QDebug dbg, const WritingSystem &key)
{
    if( key.isNull() )
        dbg.nospace() << "WritingSystem(null)";
    else
        dbg.nospace() << "WritingSystem(" << key.id() << ", " << key.name() << ", " << key.flexString() << key.abbreviation() << ", " <<  key.keyboardCommand() << ", " << key.fontFamily() << ", " << key.fontSize() << ")";
    return dbg.maybeSpace();
}
