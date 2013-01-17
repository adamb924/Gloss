#include "morphologicalanalysis.h"
#include "allomorph.h"

#include <QStringList>

MorphologicalAnalysis::MorphologicalAnalysis()
{
    mTextFormId = -1;
    mWritingSystem = WritingSystem();
}

MorphologicalAnalysis::MorphologicalAnalysis(const TextBit & textForm)
{
    mTextFormId = textForm.id();
    mWritingSystem = textForm.writingSystem();
}

MorphologicalAnalysis::MorphologicalAnalysis(qlonglong textFormId, const WritingSystem & ws)
{
    mTextFormId = textFormId;
    mWritingSystem = ws;
}

MorphologicalAnalysis::MorphologicalAnalysis(const MorphologicalAnalysis & other)
{
    mTextFormId = other.mTextFormId;
    mAllomorphs = other.mAllomorphs;
}

MorphologicalAnalysis& MorphologicalAnalysis::operator=(const MorphologicalAnalysis & other)
{
    mTextFormId = other.mTextFormId;
    mAllomorphs = other.mAllomorphs;
    return *this;
}

QString MorphologicalAnalysis::baselineSummary() const
{
    QString summary;
    for(int i=0; i<mAllomorphs.count(); i++)
        summary += mAllomorphs.at(i).text();
    return summary;
}

QString MorphologicalAnalysis::glossSummary(const WritingSystem & ws) const
{
    QStringList summary;
    for(int i=0; i<mAllomorphs.count(); i++)
    {
        summary << mAllomorphs.at(i).gloss(ws).text();
    }
    return summary.join("-");
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

Allomorph* MorphologicalAnalysis::operator[](int i)
{
    return &mAllomorphs[i];
}

int MorphologicalAnalysis::allomorphCount() const
{
    return mAllomorphs.count();
}

WritingSystem MorphologicalAnalysis::writingSystem() const
{
    return mWritingSystem;
}
