#ifndef SYNTACTICTYPE_H
#define SYNTACTICTYPE_H

#include <QString>
#include <QKeySequence>

class SyntacticType
{
public:
    SyntacticType();
    explicit SyntacticType(const QString & abbreviation);
    SyntacticType(const QString & name, const QString & abbreviation, const QKeySequence & keySequence, const QString & automaticParent);

    QString name() const;
    QString abbreviation() const;
    QKeySequence keySequence() const;
    QString automaticParent() const;

    bool isNull() const;

private:
    QString mName;
    QString mAbbreviation;
    QKeySequence mKeySequence;
    QString mAutomaticParent;
};

QDebug operator<<(QDebug dbg, const SyntacticType &key);

/// this is not of course part of SyntacticType, but it needs to be here for this class to be used well
uint qHash(const QKeySequence & key);

#endif // SYNTACTICTYPE_H
