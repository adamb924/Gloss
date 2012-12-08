#include "phrase.h"

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

QString Phrase::gloss(const WritingSystem & ws)
{
    return mGlosses.value(ws).text();
}

TextBitHash* Phrase::glosses()
{
    return &mGlosses;
}
