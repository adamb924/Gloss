/*!
  \class Phrase
  \ingroup DataStructures
  \brief A data class holding a list of GlossItem objects, as well as a hash map with phrase-level glosses.
*/

#ifndef PHRASE_H
#define PHRASE_H

#include <QList>
#include <QHash>
#include <QObject>
#include "glossitem.h"
#include "textbit.h"

class Phrase : public QObject, public QList<GlossItem*>
{
    Q_OBJECT
public:
    Phrase();
    ~Phrase();

    //! \brief Returns a (read-only) iterator for the TextBitHash containing the phrase-level glosses. See gloss() for an alternate interface to this data structure.
    TextBitHashIterator glosses();

    //! \brief Returns the phrasal gloss for the given WritingSystem, or if this does not yet exist, creates an empty gloss and returns that.
    TextBit gloss(const WritingSystem & ws);

public slots:
    //! \brief Adds or updates the phrase-level gloss to \a bit. The gloss to be added or updated is indicated by the WritingSystem of \bit.
    void setPhrasalGloss( const TextBit & bit );

private:
    TextBitHash mGlosses;
};

#endif // PHRASE_H
