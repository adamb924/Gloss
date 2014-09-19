#include "syntactictype.h"

#include <QtDebug>

SyntacticType::SyntacticType()
{
}

SyntacticType::SyntacticType(const QString & name, const QString & abbreviation, const QKeySequence & keySequence) :
    mName(name),
    mAbbreviation(abbreviation),
    mKeySequence(keySequence)
{
}

QString SyntacticType::name() const
{
    return mName;
}

QString SyntacticType::abbreviation() const
{
    return mAbbreviation;
}

QKeySequence SyntacticType::keySequence() const
{
    return mKeySequence;
}

bool SyntacticType::isNull() const
{
    return mAbbreviation.isEmpty();
}

uint qHash(const QKeySequence &key)
{
    return qHash( key.toString() );
}


QDebug operator<<(QDebug dbg, const SyntacticType &key)
{
    if( key.isNull() )
        dbg.nospace() << "SyntacticType(null)";
    else
        dbg.nospace() << "SyntacticType(" << key.name() << ", " << key.abbreviation() << ", " <<  key.keySequence().toString() << ")";
    return dbg.maybeSpace();
}
