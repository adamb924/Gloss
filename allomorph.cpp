#include "allomorph.h"

#include <QRegExp>

Allomorph::Allomorph()
{
    mType = Null;
    mId = -1;
    mTextBit = TextBit();
}

Allomorph::Allomorph(qlonglong id, const TextBit & bit)
{
    mType = Null;
    mId = id;
    mTextBit = bit;
    setTypeFromString(bit.text());
}

Allomorph::Allomorph(qlonglong id, const TextBit & bit, const TextBitHash & glosses )
{
    mType = Null;
    mId = id;
    mTextBit = bit;
    setTypeFromString(bit.text());
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

void Allomorph::setTypeFromString(const QString & string)
{
    QRegExp rePrefix("^[^-].*-$");
    QRegExp reSuffix("^-.*[^-]$");
    QRegExp reInfix("^-.*-$");
    QRegExp reBoundStem("^\\*.*$");
    QRegExp reProclitic("^[^=].*=$");
    QRegExp reEnclitic("^=.*[^=]$");
    QRegExp reSimulfix("^=.*=$");
    QRegExp reSuprafix("^~.*~$");

    if( rePrefix.exactMatch( string ) )
        mType = Allomorph::Prefix;
    else if( reSuffix.exactMatch( string ) )
        mType = Allomorph::Suffix;
    else if( reInfix.exactMatch( string ) )
        mType = Allomorph::Infix;
    else if( reBoundStem.exactMatch( string ) )
        mType = Allomorph::BoundStem;
    else if( reProclitic.exactMatch( string ) )
        mType = Allomorph::Proclitic;
    else if( reEnclitic.exactMatch( string ) )
        mType = Allomorph::Enclitic;
    else if( reSimulfix.exactMatch( string ) )
        mType = Allomorph::Simulfix;
    else if( reSuprafix.exactMatch( string ) )
        mType = Allomorph::Suprafix;
    else
        mType = Allomorph::Stem;
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
    return mTextBit.text();
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
