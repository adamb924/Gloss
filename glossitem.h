/*!
  \class GlossItem
  \ingroup Data
  \brief A data class representing a gloss item in an interlinear text. This consists of an id (the _id field of the Interpretations SQL table), hashes of text and gloss lines, and a pointer to the TextBit of the baseline text.
*/

#ifndef GLOSSITEM_H
#define GLOSSITEM_H

#include <QObject>
#include <QHash>

#include "writingsystem.h"
#include "textbit.h"

class TextBit;

class GlossItem : public QObject
{
    Q_OBJECT
public:
    //! \brief Construct a GlossItem that is empty except for the baseline TextBit.
    explicit GlossItem(const TextBit & baselineText, QObject *parent = 0);
    //! \brief Construct a GlossItem with the given baseline TextBit, and other gloss and text forms.
    GlossItem(const TextBit & baselineText, const QList<TextBit> & textForms, const QList<TextBit> & glossForms, QObject *parent = 0);

    void setId(qlonglong id);
    qlonglong id() const;

    TextBit baselineText() const;

    QHash<WritingSystem, QString>* textItems();
    QHash<WritingSystem, QString>* glossItems();

signals:

public slots:

private:
    QHash<WritingSystem, QString> mTextItems;
    QHash<WritingSystem, QString> mGlossItems;

    qlonglong mId;
    TextBit mBaselineText;
};

#endif // GLOSSITEM_H
