/*!
  \class Allomorph
  \ingroup Data
  \brief This class represents an allomorph. Typically Allomorph objects would be contained within a MorphologicalAnalysis object.

  An Allomorph object corresponds to one row of the Allomorph SQL table.
*/

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

    //! \brief Returns the type of the allomorph as Allomorph::Type
    Type type() const;

    //! \brief Returns the type of the allomorph as a string
    QString typeString() const;

    //! \brief Returns the text of the allomorph (without punctuation indicators)
    QString text() const;

    //! \brief Returns the text of the allomorph, punctuation indicators indicating what kind of allomorph it is
    QString typeFormattedString() const;

    //! \brief Returns the string formatted with a - or a =, according to whether it is a proclitic, enclitic, prefix, suffix
    TextBit textBitForConcatenation() const;

    //! \brief Returns the text of the allomorph as a TextBit (including punctuation indicators)
    TextBit textBit() const;

    //! \brief Returns the WritingSystem of the allomorph
    WritingSystem writingSystem() const;

    //! \brief Returns true if the allomorph is a stem, otherwise false
    bool isStem() const;

    //! \brief Returns true if the allomorph is a clitic, otherwise false
    bool isClitic() const;

    //! \brief Return the gloss of the allomorph in WritingSystem \a ws
    TextBit gloss(const WritingSystem & ws) const;

    //! \brief Set all glosses of the allomorph to \a glosses. All existing glosses are lost
    void setGlosses(const TextBitHash & glosses);

    //! \brief Returns the a list of WritingSystem objects for which the allomorph has glosses
    QList<WritingSystem> glossLanguages() const;

    //! \brief Returns the id of the allomorph
    qlonglong id() const;

    //! \brief Set the id of the allomorph to \a id
    void setId(qlonglong id);

    //! \brief Set the type of the allomorph to \a t
    void setType( Type t );

    //! \brief Returns the type string for \a t
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

    //! \brief Returns the Type, given its string represenation \a t
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

    //! \brief Returns a string representation of \a text, as if it were an allomorph of type \a t
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

    //! \brief Returns the Type of an allomorph, given the string representation \a string
    static Type typeFromFormattedString(const QString & string)
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

    //! \brief Returns \a string with punctuation stripped out
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
