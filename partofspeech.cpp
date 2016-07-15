#include "partofspeech.h"

#include <QtDebug>

PartOfSpeech::PartOfSpeech()
{

}

PartOfSpeech::PartOfSpeech(const QString & name, const QString & abbreviation, const QKeySequence & keySequence) :
    mName(name),
    mAbbreviation(abbreviation),
    mKeySequence(keySequence)
{
}

QString PartOfSpeech::name() const
{
    return mName;
}

QString PartOfSpeech::abbreviation() const
{
    return mAbbreviation;
}

QKeySequence PartOfSpeech::keySequence() const
{
    return mKeySequence;
}

bool PartOfSpeech::isNull() const
{
    return mAbbreviation.isEmpty();
}

bool PartOfSpeech::operator==(const PartOfSpeech &other) const
{
    return mName == other.mName;
}

QDebug operator<<(QDebug dbg, const PartOfSpeech &key)
{
    if( key.isNull() )
        dbg.nospace() << "PartOfSpeech(null)";
    else
        dbg.nospace() << "PartOfSpeech( Name: " << key.name() << ", Abbreviation: " << key.abbreviation() << ", Shortcut: " <<  key.keySequence().toString() << ")";
    return dbg.maybeSpace();
}
