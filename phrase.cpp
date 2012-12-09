#include "phrase.h"

#include <QtDebug>

Phrase::Phrase()
{
}

Phrase::~Phrase()
{
}

void Phrase::addGloss(const TextBit & bit)
{
    mGlosses.insert(bit.writingSystem(), bit );
}

TextBitHash* Phrase::glosses()
{
    return &mGlosses;
}

TextBit Phrase::gloss(const WritingSystem & ws)
{
    if( !mGlosses.contains(ws))
        mGlosses.insert( ws, TextBit("", ws) );
    return mGlosses.value(ws);
}

void Phrase::setPhrasalGloss( const TextBit & bit )
{
    mGlosses.insert( bit.writingSystem(), bit );
}
