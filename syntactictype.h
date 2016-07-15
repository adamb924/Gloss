/*!
  \class SyntacticType
  \ingroup Data
  \brief A data class representing the type of a syntactic analysis constituent. Used widely, it is crucially the property of SyntacticAnalysisElement. Each SyntacticType object corresponds to a row in the database table SyntacticConstituents.
*/

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

#ifndef QHASH_QKEYSEQUENCE
#define QHASH_QKEYSEQUENCE
/// this is not of course part of SyntacticType, but it needs to be here for this class to be used well
uint qHash(const QKeySequence & key);
#endif

#endif // SYNTACTICTYPE_H
