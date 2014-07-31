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
    mTextFormId(other.mTextFormId), mWritingSystem(other.mWritingSystem)
{
    for(int i=0; i<other.mAllomorphs.count(); i++)
    {
        mAllomorphs.append( new Allomorph(*other.allomorph(i)) );
    }
}

MorphologicalAnalysis::~MorphologicalAnalysis()
{
    if( mTextFormId == 3099 )
    {
        qDebug() << "~MorphologicalAnalysis()" << mAllomorphs;
    }
    qDeleteAll(mAllomorphs);
}

MorphologicalAnalysis& MorphologicalAnalysis::operator=(const MorphologicalAnalysis & other)
{
    mWritingSystem = other.mWritingSystem;
    mTextFormId = other.mTextFormId;
    for(int i=0; i<other.mAllomorphs.count(); i++)
    {
        mAllomorphs.append( new Allomorph(*other.allomorph(i)) );
    }
    return *this;
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
        if( ! mAllomorphs.at(i)->equalExceptGuid( *other.mAllomorphs.at(i) ) )
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
            if( mAllomorphs.at(i)->isClitic() )
                summary += "=";
            else
                summary += "-";
        }
        summary += mAllomorphs.at(i)->text();
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
            if( mAllomorphs.at(i)->isClitic() )
                summary += "=";
            else
                summary += "-";
        }
        summary += mAllomorphs.at(i)->gloss(ws).text();
    }
    return summary;
}

AllomorphIterator MorphologicalAnalysis::allomorphIterator() const
{
    return AllomorphIterator(mAllomorphs);
}

void MorphologicalAnalysis::addAllomorph(Allomorph * allomorph)
{
    mAllomorphs.append(allomorph);
}

bool MorphologicalAnalysis::isEmpty() const
{
    return mAllomorphs.isEmpty();
}

Allomorph* MorphologicalAnalysis::allomorph(int i)
{
    return mAllomorphs[i];
}

const Allomorph* MorphologicalAnalysis::allomorph(int i) const
{
    return mAllomorphs.at(i);
}

Allomorph* MorphologicalAnalysis::operator[](int i)
{
    return mAllomorphs[i];
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
