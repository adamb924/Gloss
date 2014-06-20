/*!
  \class Phrase
  \ingroup Data
  \brief A data class holding a list of GlossItem objects, as well as a hash map with phrase-level glosses.

  A Phrase object corresponds to a &lt;phrase&gt; element in a .flextext file. The list of GlossItem objects corresponds to &lt;word&gt; elements within the &lt;phrase&gt;. Phrasal glosses correspond to &lt;item&gt; elements that are immediate descendants of &lt;phrase&gt;.
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

    //! \brief Returns a string with all the words of the phrase delimited by spaces
    QString equivalentBaselineLineText() const;

    //! \brief Set the sound interval of the Phrase
    void setInterval( const Interval & interval );

    //! \brief Returns the sound interval of the Phrase
    Interval* interval();

    //! \brief Returns the number of gloss items in the Phrase
    int glossItemCount() const;

    //! \brief Returns a const pointer to the gloss item at \a index
    const GlossItem* glossItemAt(int index) const;

    //! \brief Returns the gloss item at \a index
    GlossItem* glossItemAt(int index);

    //! \brief Remove all GlossItem objects from the Phrase
    void clearGlossItems();

    //! \brief Returns the index of GlossItem \a item
    int indexOfGlossItem(GlossItem *item) const;

    //! \brief Connects \a item to the concordance, and relevant signals/slots in Text and Phrase. Returns \a item.
    GlossItem* connectGlossItem(GlossItem * item);

    //! \brief Appends \a item to the end of the phrase
    void appendGlossItem(GlossItem * item);

    //! \brief Returns the last GlossItem of the Phrase
    GlossItem* lastGlossItem();

    //! \brief Returns the a const pointer to a list of GlossItems
    const QList<GlossItem*>* glossItems() const;

    void connectToText();

signals:
    //! \brief Emitted whenever a GlossItem of the phrase changes
    void phraseChanged();

    //! \brief Emitted whenever a phrasal gloss changes
    void phrasalGlossChanged(Phrase * thisPhrase, const TextBit & bit);

    //! \brief Emitted when the user requests to delete the final GlossItem
    void requestRemovePhrase(Phrase * phrase);

    //! \brief Emitted whenever a GlossItem of the phrase changes
    /// @todo This is emitted every place that requestGuiRefresh() is emitted. Should it be eliminated?
    void requestGuiRefresh(Phrase * phrase);

public slots:
    //! \brief Adds or updates the phrase-level gloss to \a bit. The gloss to be added or updated is indicated by the WritingSystem of \a bit
    void setPhrasalGloss( const TextBit & bit );

    //! \brief Replace \a glossItem with new GlossItem objects constructed from \a bits. There is no restriction on the relationship between the baseline text of \a item to the contents of \a bits
    void splitGloss( GlossItem *glossItem, const QList<TextBit> & bits );

    //! \brief Merge \a glossItem with the next following GlossItem
    void mergeGlossItemWithNext( GlossItem *glossItem );

    //! \brief Merge \a glossItem with the previous GlossItem
    void mergeGlossItemWithPrevious( GlossItem *glossItem );

    //! \brief Remove \a glossItem from the phrase
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
