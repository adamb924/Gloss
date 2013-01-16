#ifndef CONCORDANCE_H
#define CONCORDANCE_H

#include <QObject>
#include <QMultiHash>

class WordDisplayWidget;
class ImmutableLabel;
class LingEdit;
class TextBit;
class GlossItem;

class Concordance : public QObject
{
    Q_OBJECT
public:
    explicit Concordance(QObject *parent = 0);

signals:

public slots:
    //! Insert \a edit into the text form LingEdit concordance, indexed by \a newId. If \a edit was previouly indexed by another id, that is removed.
    void updateTextFormLingEditConcordance(LingEdit * edit, qlonglong newId);

    //! Insert \a edit into the gloss LingEdit concordance, indexed by \a newId. If \a edit was previouly indexed by another id, that is removed.
    void updateGlossLingEditConcordance(LingEdit * edit, qlonglong newId);

    //! Removes \a edit from the gloss LingEdit concordance.
    void removeGlossFromLingEditConcordance( QObject * edit );

    //! Removes \a edit from the text form LingEdit concordance.
    void removeTextFormFromLingEditConcordance( QObject * edit );

    //! Insert \a edit into the text form LingEdit concordance, indexed by \a newId. If \a edit was previouly indexed by another id, that is removed.
    void updateTextForImmutableLabelConcordance(ImmutableLabel * edit, qlonglong newId);

    //! Insert \a edit into the gloss LingEdit concordance, indexed by \a newId. If \a edit was previouly indexed by another id, that is removed.
    void updateGlossImmutableLabelConcordance(ImmutableLabel * edit, qlonglong newId);

    //! Removes \a edit from the gloss LingEdit concordance.
    void removeGlossFromImmutableLabelConcordance( QObject * edit );

    //! Removes \a edit from the text form LingEdit concordance.
    void removeTextFormFromImmutableLabelConcordance( QObject * edit );

    //! Updates all widgets displaying the gloss indicated by \a bit (and its id())
    void updateGloss( const TextBit & bit );

    //! Updates all widgets displaying the text form indicated by \a bit (and its id())
    void updateTextForm( const TextBit & bit );

    //! Insert \a item into the GlossItem concordance, indexed by \a newId. If \a item was previouly indexed by another id, that is removed.
    void updateGlossItemConcordance(GlossItem * item, qlonglong newId);

    //! Removes \a item from the GlossItem concordance.
    void removeGlossItemFromConcordance( GlossItem * item );

    //! Alert all GlossItems in the concordance that an alternate interpretation is available
    void otherInterpretationsAvailableForGlossItem( qlonglong id );

private:
    //! \brief GlossItem objects, indexed by interpretation id
    QMultiHash<qlonglong,GlossItem*> mGlossItems;

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
