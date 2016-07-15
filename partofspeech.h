#ifndef PARTOFSPEECH_H
#define PARTOFSPEECH_H

#include <QString>
#include <QKeySequence>

class PartOfSpeech
{
public:
    PartOfSpeech();
    PartOfSpeech(const QString & name, const QString & abbreviation, const QKeySequence & keySequence);

    QString name() const;
    QString abbreviation() const;
    QKeySequence keySequence() const;

    bool isNull() const;

    bool operator==(const PartOfSpeech & other) const;

private:
    QString mName;
    QString mAbbreviation;
    QKeySequence mKeySequence;
};

QDebug operator<<(QDebug dbg, const PartOfSpeech &key);

#ifndef QHASH_QKEYSEQUENCE
#define QHASH_QKEYSEQUENCE
/// this is not of course part of SyntacticType, but it needs to be here for this class to be used well
uint qHash(const QKeySequence & key);
#endif

#endif // PARTOFSPEECH_H
