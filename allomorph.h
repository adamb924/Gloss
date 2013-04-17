#ifndef ALLOMORPH_H
#define ALLOMORPH_H

#include "textbit.h"

#include <QRegExp>

class Allomorph
{
public:
    enum Type { Stem,Prefix,Suffix,Infix,BoundStem,Proclitic,Enclitic,Simulfix,Suprafix,Null };

    Allomorph();
    Allomorph(qlonglong id, const TextBit & bit, Type type );
    Allomorph(qlonglong id, const TextBit & bit, const TextBitHash & glosses , Type type );
    Allomorph(const Allomorph & other);
    Allomorph& operator=(const Allomorph & other);

    bool operator==(const Allomorph & other) const;

    Type type() const;
    QString typeString() const;

    QString text() const;
    QString typeFormattedString() const;

    TextBit textBit() const;
    WritingSystem writingSystem() const;

    bool isStem() const;
    bool isClitic() const;

    TextBit gloss(const WritingSystem & ws) const;
    void setGlosses(const TextBitHash & glosses);
    QList<WritingSystem> glossLanguages() const;

    qlonglong id() const;
    void setId(qlonglong id);

    void setType( Type t );

    static QString getTypeString(Type t)
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

    static Type getType(QString t)
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

    static QString getTypeFormatTextString( const QString & text, Type t )
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


    static Type typeFromFormattedString(const QString & string)
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

    static QString stripPunctuation( const QString & string )
    {
        QString s = string;
        s.replace("=","");
        s.replace("~","");
        s.replace("=","");
        s.replace("*","");
        s.replace("-","");
        return s;
    }

private:
    Type mType;
    TextBit mTextBit;
    qlonglong mId;
    TextBitHash mGlosses;
};


#endif // ALLOMORPH_H
