#include "phrase.h"

Phrase::Phrase()
{
}

Phrase::~Phrase()
{
}

void Phrase::addGloss(const TextBit & bit)
{
    mGlosses.insert(bit.writingSystem(), bit.text());
}

QString Phrase::gloss(const WritingSystem & ws)
{
    return mGlosses.value(ws);
}

TextBitHash* Phrase::glosses()
{
    return &mGlosses;
}
