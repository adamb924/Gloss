#ifndef ALLOMORPH_H
#define ALLOMORPH_H

#include "textbit.h"

class Allomorph
{
public:
    enum Type { Stem,Prefix,Suffix,Infix,BoundStem,Proclitic,Enclitic,Simulfix,Suprafix,Null };

    Allomorph();
    Allomorph(qlonglong id, const TextBit & bit);
    Allomorph(qlonglong id, const TextBit & bit, const TextBitHash & glosses );
    Allomorph(const Allomorph & other);
    Allomorph& operator=(const Allomorph & other);

    bool operator==(const Allomorph & other) const;

    Type type() const;
    QString typeString() const;

    QString text() const;
    TextBit textBit() const;
    WritingSystem writingSystem() const;

    TextBit gloss(const WritingSystem & ws) const;
    void setGlosses(const TextBitHash & glosses);
    QList<WritingSystem> glossLanguages() const;

    qlonglong id() const;
    void setId(qlonglong id);

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

private:
    void setTypeFromString( const QString & string );

    Type mType;
    TextBit mTextBit;
    qlonglong mId;
    TextBitHash mGlosses;
};


#endif // ALLOMORPH_H
