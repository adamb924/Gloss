#include "paragraph.h"

#include "phrase.h"

Paragraph::Paragraph(QObject *parent) :
    QObject(parent)
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
    foreach(Phrase * phrase, mPhrases)
    {
        int index = phrase->indexOfGlossItem(item);
        if( index != -1 )
            return index;
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
}
