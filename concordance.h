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
    //! Removes \a item from the GlossItem concordance and the GlossItem by TextForm id concordance.
    void removeGlossItemFromConcordance( QObject * item );

    void removeGlossItemTextFormIdPair(GlossItem *item , qlonglong textFormId );

    void removeGlossItemGlossIdPair(GlossItem *item , qlonglong glossId );

    //! Updates all widgets displaying the gloss indicated by \a bit (and its id())
    void updateGloss( const TextBit & bit );

    //! Updates all widgets displaying the text form indicated by \a bit (and its id())
    void updateTextForm( const TextBit & bit );

    //! Alert all GlossItems in the concordance that an alternate interpretation is available
    void updateInterpretationsAvailableForGlossItem( GlossItem::CandidateNumber mCandidateNumber, qlonglong textFormId );

    //! Insert \a item into the GlossItem by TextForm id concordance, indexed by \a textFormId. If \a item was previouly indexed by another id, that is removed.
    void updateGlossItemTextFormConcordance( GlossItem * item, qlonglong textFormId );

    //! Insert \a item into the GlossItem by gloss id concordance, indexed by \a glossId. If \a item was previouly indexed by another id, that is removed.
    void updateGlossItemGlossConcordance( GlossItem * item, qlonglong glossId );

    //! Updates gloss items with \a textFormId so that they contain \a analysis
    void updateGlossItemMorphologicalAnalysis( const MorphologicalAnalysis & analysis );

private:
    //! \brief GlossItem objects, indexed by text form id
    QMultiHash<qlonglong,GlossItem*> mGlossItemsByTextFormId;

    //! \brief GlossItem objects, indexed by gloss form id
    QMultiHash<qlonglong,GlossItem*> mGlossItemsByGlossId;
};

#endif // CONCORDANCE_H
