#include "allomorph.h"

#include "concordance.h"

#include <QRegExp>
#include <QtDebug>


Allomorph::Allomorph(QUuid guid) :
    QObject(), mType(Allomorph::Null), mTextBit( TextBit() ), mId(-1), mLexicalEntryId(-1), mGuid(guid)
{
    if( mGuid.isNull() )
    {
        mGuid = QUuid::createUuid();
    }
}

Allomorph::Allomorph(qlonglong id, qlonglong lexicalEntryId, const TextBit & bit, Type type , QUuid guid) :
    QObject(), mType(type), mTextBit(bit), mId(id), mLexicalEntryId(lexicalEntryId), mGuid(guid)
{
    if( mGuid.isNull() )
    {
        mGuid = QUuid::createUuid();
    }
}

Allomorph::Allomorph(qlonglong id, qlonglong lexicalEntryId, const TextBit & bit, const TextBitHash & glosses , Type type, QUuid guid  ) :
    QObject(), mType(type), mTextBit(bit), mId(id), mLexicalEntryId(lexicalEntryId), mGuid(guid)
{
    mGlosses.unite(glosses);
    if( mGuid.isNull() )
    {
        mGuid = QUuid::createUuid();
    }
}

Allomorph::Allomorph(const Allomorph & other) :
    QObject(), mType(other.mType), mTextBit(other.mTextBit), mId(other.mId), mLexicalEntryId(other.mLexicalEntryId), mGlosses(other.mGlosses), mGuid(other.mGuid)
{
}

Allomorph& Allomorph::operator=(const Allomorph & other)
{
    mType = other.mType;
    mId = other.mId;
    mLexicalEntryId = other.mLexicalEntryId;
    mTextBit = other.mTextBit;
    mGlosses = other.mGlosses;
    mGuid = other.mGuid;
    return *this;
}

Allomorph::~Allomorph()
{
    emit allomorphDestroyed(this);
}

void Allomorph::connectToConcordance(Concordance *concordance)
{
    concordance->insertIntoAllomorphConcordance( this );
    connect( this, SIGNAL(allomorphDestroyed(Allomorph*)), concordance, SLOT(removeFromAllomorphConcordance(Allomorph*)) );
    connect( this, SIGNAL(glossesChanged(Allomorph*)), concordance, SLOT(updateAllomorphTextForms(Allomorph*)) );
}

bool Allomorph::operator==(const Allomorph & other) const
{
    return mType == other.mType && mTextBit == other.mTextBit && mId == other.mId && mGlosses == other.mGlosses && mGuid == other.mGuid;
}

bool Allomorph::equalExceptGuid(const Allomorph &other) const
{
    return mType == other.mType && mTextBit == other.mTextBit && mId == other.mId && mGlosses == other.mGlosses;
}

bool Allomorph::equalExceptGuid(const Allomorph * other) const
{
    return mType == other->mType && mTextBit == other->mTextBit && mId == other->mId && mGlosses == other->mGlosses;
}

QString Allomorph::typeFormattedString() const
{
    return mTextBit.text();
}

TextBit Allomorph::textBitForConcatenation() const
{
    QString text = mTextBit.text();
    if( mType == Allomorph::Proclitic )
    {
        text = text + "=";
    }
    else if ( mType == Allomorph::Enclitic )
    {
        text = "=" + text;
    }
    else if ( mType == Allomorph::Suffix )
    {
        text = "-" + text;
    }
    else if ( mType == Allomorph::Prefix )
    {
        text = text + "-";
    }
    return TextBit( text, mTextBit.writingSystem() );
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

qlonglong Allomorph::lexicalEntryId() const
{
    return mLexicalEntryId;
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
    if( mGlosses != glosses )
    {
        mGlosses = glosses;
        emit glossesChanged(this);
    }
}

QList<WritingSystem> Allomorph::glossLanguages() const
{
    return mGlosses.keys();
}

TextBitHash Allomorph::glosses() const
{
    return mGlosses;
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

QString Allomorph::guid() const
{
    return mGuid.toString();
}

void Allomorph::setGuid(const QString &guidString)
{
    mGuid = QUuid(guidString);
}

void Allomorph::setGuid(const QUuid &guid)
{
    mGuid = guid;
}

QString Allomorph::getTypeString(Allomorph::Type t)
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
        return "Null";
    }
    return "Null";
}

Allomorph::Type Allomorph::getType(QString t)
{
    if( t == "Stem" )
        return Stem;
    else if( t == "Prefix" )
        return Prefix;
    else if( t == "Suffix" )
        return Suffix;
    else if( t == "Infix" )
        return Infix;
    else if( t == "Bound Stem" )
        return BoundStem;
    else if( t == "Proclitic" )
        return Proclitic;
    else if( t == "Enclitic" )
        return Enclitic;
    else if( t == "Simulfix" )
        return Simulfix;
    else if( t == "Suprafix" )
        return Suprafix;
    else
        return Null;
}

QString Allomorph::getTypeFormatTextString(const QString &text, Allomorph::Type t)
{
    switch(t)
    {
    case Stem:
        return text;
    case Prefix:
        return text + "-";
    case Suffix:
        return "-" + text;
    case Infix:
        return "-" + text + "-";
    case BoundStem:
        return "*" + text;
    case Proclitic:
        return text + "=";
    case Enclitic:
        return "=" + text;
    case Simulfix:
        return "=" + text + "=";
    case Suprafix:
        return "~" + text + "~";
    case Null:
        return text;
    }
    return text;
}

TextBit Allomorph::getTypeFormatTextBit(const TextBit &bit, Allomorph::Type t)
{
    return TextBit( getTypeFormatTextString( bit.text(), t ) , bit.writingSystem() );
}

Allomorph::Type Allomorph::typeFromFormattedString(const QString &string)
{
    QRegExp rePrefix("^[^-].*-$");
    QRegExp reSuffix("^-.*[^-]$");
    QRegExp reInfix("^-.*-$");
    QRegExp reBoundStem("^\\*.*$");
    QRegExp reProclitic("^[^=].*={1,2}$");
    QRegExp reEnclitic("^={1,2}.*[^=]$");
    QRegExp reSimulfix("^={1,2}.*={1,2}$");
    QRegExp reSuprafix("^~.*~$");

    if( rePrefix.exactMatch( string ) )
        return Allomorph::Prefix;
    else if( reSuffix.exactMatch( string ) )
        return Allomorph::Suffix;
    else if( reInfix.exactMatch( string ) )
        return Allomorph::Infix;
    else if( reBoundStem.exactMatch( string ) )
        return Allomorph::BoundStem;
    else if( reProclitic.exactMatch( string ) )
        return Allomorph::Proclitic;
    else if( reEnclitic.exactMatch( string ) )
        return Allomorph::Enclitic;
    else if( reSimulfix.exactMatch( string ) )
        return Allomorph::Simulfix;
    else if( reSuprafix.exactMatch( string ) )
        return Allomorph::Suprafix;
    else
        return Allomorph::Stem;
}

Allomorph::Type Allomorph::typeFromFormattedString(const TextBit &bit)
{
    return typeFromFormattedString(bit.text());
}

QString Allomorph::stripPunctuation(const QString &string)
{
    QString s = string;
    s.replace("=","");
    s.replace("~","");
    s.replace("=","");
    s.replace("*","");
    s.replace("-","");
    return s;
}

TextBit Allomorph::stripPunctuationFromTextBit(const TextBit &bit)
{
    TextBit retVal = bit;
    retVal.setText( stripPunctuation( bit.text()) );
    return retVal;
}

QDebug operator<<(QDebug dbg, const Allomorph &key)
{
    dbg.nospace() << "Allomorph(" << key.id() << ", " << key.textBit() << ", " << key.guid();
    return dbg.maybeSpace();
}
