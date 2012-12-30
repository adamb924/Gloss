#ifndef ALLOMORPH_H
#define ALLOMORPH_H

#include "textbit.h"

class Allomorph
{
public:
    enum Type { Stem,Prefix,Suffix,Infix,BoundStem,Proclitic,Enclitic,Simulfix,Suprafix };

    Allomorph();
    Allomorph(const TextBit & bit);

    Type type() const;
    QString typeString() const;

    QString text() const;

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
        }
        return "";
    }

private:
    void setTypeFromString( const QString & string );

    Type mType;
    TextBit mTextBit;
};

typedef QList<Allomorph> MorphologicalAnalysis;
typedef QListIterator<Allomorph> MorphologicalAnalysisIterator;

#endif // ALLOMORPH_H
