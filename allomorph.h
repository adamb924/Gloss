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

    QString getTypeString(Type t) const;
    QString getTypeFormatTextString( const QString & text, Type t ) const;

private:
    void setTypeFromString( const QString & string );
    QString stripPunctuation( const QString & string ) const;

    Type mType;
    TextBit mTextBit;
    qlonglong mId;
    TextBitHash mGlosses;
};


#endif // ALLOMORPH_H
