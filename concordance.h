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
    //! Insert \a edit into the text form LingEdit concordance, indexed by \a newTextFormId. If \a edit was previouly indexed by another id, that is removed.
    void updateTextFormLingEditConcordance(LingEdit * edit, qlonglong newTextFormId);

    //! Insert \a edit into the gloss LingEdit concordance, indexed by \a newGlossId. If \a edit was previouly indexed by another id, that is removed.
    void updateGlossLingEditConcordance(LingEdit * edit, qlonglong newGlossId);

    //! Removes \a edit from the gloss LingEdit concordance.
    void removeGlossFromLingEditConcordance( QObject * edit );

    //! Removes \a edit from the text form LingEdit concordance.
    void removeTextFormFromLingEditConcordance( QObject * edit );

    //! Insert \a edit into the text form LingEdit concordance, indexed by \a newTextFormId. If \a edit was previouly indexed by another id, that is removed.
    void updateTextForImmutableLabelConcordance(ImmutableLabel * edit, qlonglong newTextFormId);

    //! Insert \a edit into the gloss LingEdit concordance, indexed by \a newGlossId. If \a edit was previouly indexed by another id, that is removed.
    void updateGlossImmutableLabelConcordance(ImmutableLabel * edit, qlonglong newGlossId);

    //! Removes \a edit from the gloss LingEdit concordance.
    void removeGlossFromImmutableLabelConcordance( QObject * edit );

    //! Removes \a edit from the text form LingEdit concordance.
    void removeTextFormFromImmutableLabelConcordance( QObject * edit );

    //! Updates all widgets displaying the gloss indicated by \a bit (and its id())
    void updateGloss( const TextBit & bit );

    //! Updates all widgets displaying the text form indicated by \a bit (and its id())
    void updateTextForm( const TextBit & bit );

    //! Insert \a item into the GlossItem concordance, indexed by \a newGlossItemId. If \a item was previouly indexed by another id, that is removed.
    void updateGlossItemConcordance(GlossItem * item, qlonglong newGlossItemId);

    //! Removes \a item from the GlossItem concordance and the GlossItem by TextForm id concordance.
    void removeGlossItemFromConcordance( QObject * item );

    //! Alert all GlossItems in the concordance that an alternate interpretation is available
    void updateInterpretationsAvailableForGlossItem( GlossItem::CandidateNumber mCandidateNumber, qlonglong textFormId );

    //! Insert \a item into the GlossItem by TextForm id concordance, indexed by \a textFormId. If \a item was previouly indexed by another id, that is removed.
    void updateGlossItemTextFormConcordance( GlossItem * item, qlonglong textFormId );

    //! Updates gloss items with \a textFormId so that they contain \a analysis
    void updateGlossItemMorphologicalAnalysis( const MorphologicalAnalysis & analysis, qlonglong textFormId );

private:
    //! \brief GlossItem objects, indexed by interpretation id
    QMultiHash<qlonglong,GlossItem*> mGlossItemsById;

    //! \brief GlossItem objects, indexed by text form id
    QMultiHash<qlonglong,GlossItem*> mGlossItemsByTextFormId;

    //! \brief LingEdit objects for text forms, indexed by text form id
    QMultiHash<qlonglong,LingEdit*> mTextFormLingEdits;

    //! \brief LingEdit objects for glosses, indexed by text form id
    QMultiHash<qlonglong,LingEdit*> mGlossLingEdits;

    //! \brief ImmutableLabel objects for text forms, indexed by text form id
    QMultiHash<qlonglong,ImmutableLabel*> mTextFormImmutableLabels;

    //! \brief ImmutableLabel objects for glosses, indexed by text form id
    QMultiHash<qlonglong,ImmutableLabel*> mGlossImmutableLabels;
};

#endif // CONCORDANCE_H
