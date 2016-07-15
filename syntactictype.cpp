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

#ifndef QHASH_QKEYSEQUENCE_DEFINITION
#define QHASH_QKEYSEQUENCE_DEFINITION

uint qHash(const QKeySequence &key)
{
    return qHash( key.toString() );
}

#endif
