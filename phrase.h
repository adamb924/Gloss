/*!
  \class Phrase
  \ingroup Data
  \brief A data class holding a list of GlossItem objects, as well as a hash map with phrase-level glosses.
*/

#ifndef PHRASE_H
#define PHRASE_H

#include <QList>
#include <QHash>
#include "glossitem.h"
#include "textbit.h"

class Phrase : public QList<GlossItem*>
{
public:
    Phrase();
    ~Phrase();

    void addGloss(const TextBit & bit);
    QString gloss(const WritingSystem & ws);
    QHash<WritingSystem, QString>* glosses();

private:
    QHash<WritingSystem, QString> mGlosses;
};

#endif // PHRASE_H
