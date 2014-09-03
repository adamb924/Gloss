/*!
  \class Concordance
  \ingroup GUI
  \brief This class maintains a concordance of GlossItem objects by text form id and gloss id. The purpose of this is to synchonize the changes made to text forms or glosses by updating all editors through the GUI, rather than through the database. This is currently instantiated only in Project.
*/

#ifndef CONCORDANCE_H
#define CONCORDANCE_H

#include <QObject>
#include <QMultiHash>

class WordDisplayWidget;
class ImmutableLabel;
class LingEdit;
class TextBit;
class AnalysisWidget;
class MorphologicalAnalysis;

#include "glossitem.h"

class Concordance : public QObject
{
    Q_OBJECT
public:
    explicit Concordance(QObject *parent = 0);

signals:

public slots:
    //! \brief Removes \a item from the GlossItem concordance and the GlossItem by TextForm id concordance.
    void removeGlossItemFromConcordance( QObject * item );

    void removeGlossItemTextFormIdPair(GlossItem *item , qlonglong textFormId );

    void removeGlossItemGlossIdPair(GlossItem *item , qlonglong glossId );

    //! Updates all widgets displaying the gloss indicated by \a bit (and its id())
    void updateGloss( const TextBit & bit );

    //! \brief Updates all widgets displaying the text form indicated by \a bit (and its id())
    void updateTextForm( const TextBit & bit );

    //! \brief Alert all GlossItems in the concordance that an alternate interpretation is available
    void updateInterpretationsAvailableForGlossItem( GlossItem::CandidateNumber mCandidateNumber, qlonglong textFormId );

    //! \brief Insert \a item into the GlossItem by TextForm id concordance, indexed by \a textFormId. If \a item was previouly indexed by another id, that is removed.
    void updateGlossItemTextFormConcordance( GlossItem * item, qlonglong textFormId );

    //! \brief Insert \a item into the GlossItem by gloss id concordance, indexed by \a glossId. If \a item was previouly indexed by another id, that is removed.
    void updateGlossItemGlossConcordance( GlossItem * item, qlonglong glossId );

    //! \brief Updates gloss items with \a textFormId so that they contain \a analysis
    void updateGlossItemMorphologicalAnalysis(const GlossItem * originator, const MorphologicalAnalysis *analysis );

private:
    //! \brief GlossItem objects, indexed by text form id
    QMultiHash<qlonglong,GlossItem*> mGlossItemsByTextFormId;

    //! \brief GlossItem objects, indexed by gloss form id
    QMultiHash<qlonglong,GlossItem*> mGlossItemsByGlossId;
};

#endif // CONCORDANCE_H
