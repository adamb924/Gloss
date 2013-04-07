#include "allomorph.h"

#include <QRegExp>
#include <QtDebug>

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
    setTypeFromString(mTextBit.text());
}

Allomorph::Allomorph(qlonglong id, const TextBit & bit, const TextBitHash & glosses )
{
    mType = Null;
    mId = id;
    mTextBit = bit;
    setTypeFromString(mTextBit.text());
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

QString Allomorph::stripPunctuation( const QString & string ) const
{
    QString s = string;
    s.replace("=","");
    s.replace("~","");
    s.replace("=","");
    s.replace("*","");
    s.replace("-","");
    return s;
}

void Allomorph::setType( Type t )
{
    mTextBit.setText( getTypeFormatTextString( text(), t ) );
    mType = t;
}


QString Allomorph::getTypeString(Type t) const
{
    switch(t)
    {
    case Stem:
        return "Stem";
    case Prefix:
        return "Prefix";
    case Suffix:
        return "Suffix";
    case Infix:
        return "Infix";
    case BoundStem:
        return "Bound Stem";
    case Proclitic:
        return "Proclitic";
    case Enclitic:
        return "Enclitic";
    case Simulfix:
        return "Simulfix";
    case Suprafix:
        return "Suprafix";
    case Null:
    default:
        return "Null";
    }
}

QString Allomorph::getTypeFormatTextString( const QString & text, Type t ) const
{
    switch(t)
    {
    case Stem:
        return text;
        break;
    case Prefix:
        return text + "-";
        break;
    case Suffix:
        return "-" + text;
        break;
    case Infix:
        return "-" + text + "-";
        break;
    case BoundStem:
        return "*" + text;
        break;
    case Proclitic:
        return text + "=";
        break;
    case Enclitic:
        return "=" + text;
        break;
    case Simulfix:
        return "=" + text + "=";
        break;
    case Suprafix:
        return "~" + text + "~";
        break;
    case Null:
        return text;
        break;
    }
    return text;
}
