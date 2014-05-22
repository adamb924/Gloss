/*!
  \class Phrase
  \ingroup Data
  \brief A data class holding a list of GlossItem objects, as well as a hash map with phrase-level glosses.
*/

#ifndef PHRASE_H
#define PHRASE_H

#include <QList>
#include <QHash>
#include <QObject>
#include "glossitem.h"
#include "textbit.h"
#include "interval.h"

class DatabaseAdapter;
class Project;
class Text;
class Concordance;

class Phrase : public QObject
{
    Q_OBJECT
public:
    Phrase(Text *text, Project *project);
    ~Phrase();

    //! \brief Returns a (read-only) iterator for the TextBitHash containing the phrase-level glosses. See gloss() for an alternate interface to this data structure.
    TextBitHashIterator glosses();

    //! \brief Returns the phrasal gloss for the given WritingSystem, or if this does not yet exist, creates an empty gloss and returns that.
    TextBit gloss(const WritingSystem & ws);

    //! \brief Returns a text string that is the
    QString equivalentBaselineLineText() const;

    void setAnnotation( const Interval & annotation );
    Interval* annotation();

    int glossItemCount() const;
    const GlossItem* glossItemAt(int index) const;
    GlossItem* glossItemAt(int index);
    void clearGlossItems();
    int indexOfGlossItem(GlossItem *item) const;

    GlossItem* connectGlossItem(GlossItem * item);
    void appendGlossItem(GlossItem * item);
    GlossItem* lastGlossItem();
    const QList<GlossItem*>* glossItems() const;

signals:
    void phraseChanged();
    void glossChanged();
    void requestRemovePhrase(Phrase * phrase);
    void requestGuiRefresh(Phrase * phrase);

public slots:
    //! \brief Adds or updates the phrase-level gloss to \a bit. The gloss to be added or updated is indicated by the WritingSystem of \a bit.
    void setPhrasalGloss( const TextBit & bit );

    void splitGloss( GlossItem *glossItem, const QList<TextBit> & bits );
    void mergeGlossItemWithNext( GlossItem *glossItem );
    void mergeGlossItemWithPrevious( GlossItem *glossItem );
    void removeGlossItem( GlossItem *glossItem );

private:
    QList<GlossItem*> mGlossItems;
    Text *mText;
    Project *mProject;
    DatabaseAdapter *mDbAdapter;
    Concordance *mConcordance;
    Interval mAnnotation;
    TextBitHash mGlosses;
};

#endif // PHRASE_H
