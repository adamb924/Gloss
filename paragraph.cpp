#include "paragraph.h"

#include "phrase.h"

Paragraph::Paragraph(WritingSystem &baselineWritingSystem, QObject *parent) :
    QObject(parent),
    mHeader(baselineWritingSystem)
{
}

QList<Phrase*>* Paragraph::phrases()
{
    return &mPhrases;
}

const QList<Phrase*>* Paragraph::phrases() const
{
    return &mPhrases;
}

bool Paragraph::removePhrase( Phrase * phrase )
{
    int index = mPhrases.indexOf(phrase);
    if( index != -1)
    {
        delete mPhrases.takeAt(index);
        emit changed();
        return true;
    }
    return false;
}

int Paragraph::indexOf(const Phrase *phrase) const
{
    return mPhrases.indexOf(const_cast<Phrase*>(phrase));
}

int Paragraph::indexOfGlossItem(const GlossItem *item) const
{
    for(int i=0; i<mPhrases.count(); i++)
    {
        int index = mPhrases.at(i)->indexOfGlossItem(item);
        if( index != -1 )
            return i;
    }
    return -1;
}

int Paragraph::phraseCount() const
{
    return mPhrases.count();
}

void Paragraph::insertPhrase(int i, Phrase *phrase)
{
    mPhrases.insert(i, phrase);
    emit changed();
}

TextBit Paragraph::header() const
{
    return mHeader;
}

void Paragraph::setHeader(const TextBit &header)
{
    mHeader = header;
    emit changed();
}
