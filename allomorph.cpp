#include "allomorph.h"

#include <QRegExp>
#include <QtDebug>

Allomorph::Allomorph()
{
    mType = Null;
    mId = -1;
    mTextBit = TextBit();
}

Allomorph::Allomorph(qlonglong id, const TextBit & bit, Type type )
{
    mType = type;
    mId = id;
    mTextBit = bit;
}

Allomorph::Allomorph(qlonglong id, const TextBit & bit, const TextBitHash & glosses , Type type )
{
    mType = type;
    mId = id;
    mTextBit = bit;
    mGlosses = glosses;
}

Allomorph::Allomorph(const Allomorph & other)
{
    mType = other.mType;
    mId = other.mId;
    mTextBit = other.mTextBit;
    mGlosses = other.mGlosses;
}

Allomorph& Allomorph::operator=(const Allomorph & other)
{
    mType = other.mType;
    mId = other.mId;
    mTextBit = other.mTextBit;
    mGlosses = other.mGlosses;
    return *this;
}

bool Allomorph::operator==(const Allomorph & other) const
{
    return mType == other.mType && mTextBit == other.mTextBit && mId == other.mId && mGlosses == other.mGlosses;
}

QString Allomorph::typeFormattedString() const
{
    return mTextBit.text();
}

Allomorph::Type Allomorph::type() const
{
    return mType;
}

QString Allomorph::typeString() const
{
    return getTypeString(mType);
}

QString Allomorph::text() const
{
    return stripPunctuation(mTextBit.text());
}

TextBit Allomorph::textBit() const
{
    return mTextBit;
}

WritingSystem Allomorph::writingSystem() const
{
    return mTextBit.writingSystem();
}

qlonglong Allomorph::id() const
{
    return mId;
}

void Allomorph::setId(qlonglong id)
{
    mId = id;
}

TextBit Allomorph::gloss(const WritingSystem & ws) const
{
    return mGlosses.value(ws);
}

void Allomorph::setGlosses(const TextBitHash & glosses)
{
    mGlosses = glosses;
}

QList<WritingSystem> Allomorph::glossLanguages() const
{
    return mGlosses.keys();
}

bool Allomorph::isStem() const
{
    return mType == Allomorph::Stem || mType == Allomorph::BoundStem;
}

bool Allomorph::isClitic() const
{
    return mType == Allomorph::Proclitic || mType == Allomorph::Enclitic;
}

void Allomorph::setType( Type t )
{
    mType = t;
}
