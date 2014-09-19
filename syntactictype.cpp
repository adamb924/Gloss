#include "syntactictype.h"

#include <QtDebug>

SyntacticType::SyntacticType()
{
}

SyntacticType::SyntacticType(const QString &abbreviation) :
    mAbbreviation(abbreviation)
{
}

SyntacticType::SyntacticType(const QString & name, const QString & abbreviation, const QKeySequence & keySequence, const QString &automaticParent) :
    mName(name),
    mAbbreviation(abbreviation),
    mKeySequence(keySequence),
    mAutomaticParent(automaticParent)
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

QString SyntacticType::automaticParent() const
{
    return mAutomaticParent;
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
        dbg.nospace() << "SyntacticType( Name: " << key.name() << ", Abbreviation: " << key.abbreviation() << ", Shortcut: " <<  key.keySequence().toString() << ", Automatic Parent: " << key.automaticParent() << ")";
    return dbg.maybeSpace();
}
