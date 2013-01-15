#ifndef CONCORDANCE_H
#define CONCORDANCE_H

#include <QObject>
#include <QMultiHash>

class WordDisplayWidget;
class ImmutableLabel;
class LingEdit;

class Concordance : public QObject
{
    Q_OBJECT
public:
    explicit Concordance(QObject *parent = 0);

signals:

public slots:

private:
    //! \brief Word displayed widget, indexed by interpretation id
    QMultiHash<qlonglong,WordDisplayWidget*> mWdwByInterpretationId;

    //! \brief A concordance of analysis widgets, indexed by TextForm id
    QMultiHash<qlonglong,WordDisplayWidget*> mAnalysisWidgets;

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
