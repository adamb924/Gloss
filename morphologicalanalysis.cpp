#include "morphologicalanalysis.h"
#include "allomorph.h"

#include <QStringList>
#include <QDebug>

MorphologicalAnalysis::MorphologicalAnalysis() :
    mTextFormId(-1), mWritingSystem(WritingSystem())
{
}

MorphologicalAnalysis::MorphologicalAnalysis(const TextBit & textForm) :
    mTextFormId(textForm.id()), mWritingSystem(textForm.writingSystem())
{
}

MorphologicalAnalysis::MorphologicalAnalysis(qlonglong textFormId, const WritingSystem & ws) :
    mTextFormId(textFormId), mWritingSystem(ws)
{
}

MorphologicalAnalysis::MorphologicalAnalysis(const MorphologicalAnalysis & other) :
    mTextFormId(other.mTextFormId), mWritingSystem(other.mWritingSystem), mAllomorphs(other.mAllomorphs)
{
}

MorphologicalAnalysis& MorphologicalAnalysis::operator=(const MorphologicalAnalysis & other)
{
    mWritingSystem = other.mWritingSystem;
    mTextFormId = other.mTextFormId;
    mAllomorphs = other.mAllomorphs;
    return *this;
}

MorphologicalAnalysis *MorphologicalAnalysis::copyWithNewGuids() const
{
    MorphologicalAnalysis *other = new MorphologicalAnalysis(mTextFormId, mWritingSystem);
    for(int i=0; i<mAllomorphs.count(); i++)
    {
        other->addAllomorph( Allomorph(mAllomorphs.at(i).id(), mAllomorphs.at(i).textBit(), mAllomorphs.at(i).glosses(), mAllomorphs.at(i).type() ) );
    }
    return other;
}

bool MorphologicalAnalysis::operator!=(const MorphologicalAnalysis & other) const
{
    return mTextFormId != other.mTextFormId || mAllomorphs != other.mAllomorphs;
}

bool MorphologicalAnalysis::equalExceptGuid(const MorphologicalAnalysis &other) const
{
    if( mTextFormId != other.mTextFormId || mAllomorphs.count() != other.mAllomorphs.count() )
    {
        return false;
    }
    for(int i=0; i<mAllomorphs.count(); i++)
    {
        if( ! mAllomorphs.at(i).equalExceptGuid( other.mAllomorphs.at(i) ) )
        {
            return false;
        }
    }
    return true;
}

QString MorphologicalAnalysis::baselineSummary() const
{
    QString summary;
    for(int i=0; i<mAllomorphs.count(); i++)
    {
        if( i > 0 )
        {
            if( mAllomorphs.at(i).isClitic() )
                summary += "=";
            else
                summary += "-";
        }
        summary += mAllomorphs.at(i).text();
    }
    return summary;
}

QString MorphologicalAnalysis::glossSummary(const WritingSystem & ws) const
{
    QString summary;
    for(int i=0; i<mAllomorphs.count(); i++)
    {
        if( i > 0 )
        {
            if( mAllomorphs.at(i).isClitic() )
                summary += "=";
            else
                summary += "-";
        }
        summary += mAllomorphs.at(i).gloss(ws).text();
    }
    return summary;
}

AllomorphIterator MorphologicalAnalysis::allomorphIterator() const
{
    return QListIterator<Allomorph>(mAllomorphs);
}

void MorphologicalAnalysis::addAllomorph(const Allomorph & allomorph)
{
    mAllomorphs.append(allomorph);
}

bool MorphologicalAnalysis::isEmpty() const
{
    return mAllomorphs.isEmpty();
}

Allomorph* MorphologicalAnalysis::allomorph(int i)
{
    return &mAllomorphs[i];
}

const Allomorph* MorphologicalAnalysis::allomorph(int i) const
{
    return &mAllomorphs[i];
}

int MorphologicalAnalysis::allomorphCount() const
{
    return mAllomorphs.count();
}

qlonglong MorphologicalAnalysis::textFormId() const
{
    return mTextFormId;
}

void MorphologicalAnalysis::setTextFormId(qlonglong textFormId)
{
    mTextFormId = textFormId;
}

WritingSystem MorphologicalAnalysis::writingSystem() const
{
    return mWritingSystem;
}
