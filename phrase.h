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
#include "annotation.h"

class Sound;
class DatabaseAdapter;
class Project;

class Phrase : public QObject, public QList<GlossItem*>
{
    Q_OBJECT
public:
    Phrase(Project *project);
    ~Phrase();

    //! \brief Returns a (read-only) iterator for the TextBitHash containing the phrase-level glosses. See gloss() for an alternate interface to this data structure.
    TextBitHashIterator glosses();

    //! \brief Returns the phrasal gloss for the given WritingSystem, or if this does not yet exist, creates an empty gloss and returns that.
    TextBit gloss(const WritingSystem & ws);

    //! \brief Returns a text string that is the
    QString equivalentBaselineLineText() const;

    //! \brief Returns true if the Phrase thinks that the GUI should update itself.
    bool guiRefreshRequest() const;

    //! \brief Set whether the Phrase should request a GUI update or not.
    void setGuiRefreshRequest(bool needed);

    void setAnnotation( const Annotation & annotation );
    Annotation* annotation();

signals:
    void phraseChanged();

public slots:
    //! \brief Adds or updates the phrase-level gloss to \a bit. The gloss to be added or updated is indicated by the WritingSystem of \bit.
    void setPhrasalGloss( const TextBit & bit );

    void splitGlossInTwo( GlossItem *glossItem, const TextBit & wordOne, const TextBit & wordTwo );

private:
    Q_DISABLE_COPY(Phrase)

    Project *mProject;
    DatabaseAdapter *mDbAdapter;
    Annotation mAnnotation;
    TextBitHash mGlosses;
    bool mRequestGuiRefresh;
};

#endif // PHRASE_H
