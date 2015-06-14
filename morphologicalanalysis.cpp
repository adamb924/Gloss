#include "morphologicalanalysis.h"
#include "allomorph.h"

#include <QStringList>
#include <QDebug>

MorphologicalAnalysis::MorphologicalAnalysis(Concordance *concordance) :
    mTextFormId(-1), mWritingSystem(WritingSystem()), mConcordance(concordance)
{
}

MorphologicalAnalysis::MorphologicalAnalysis(const TextBit & textForm, Concordance * concordance) :
    mTextFormId(textForm.id()), mWritingSystem(textForm.writingSystem()), mConcordance(concordance)
{
}

MorphologicalAnalysis::MorphologicalAnalysis(qlonglong textFormId, const WritingSystem & ws, Concordance *concordance) :
    mTextFormId(textFormId), mWritingSystem(ws), mConcordance(concordance)
{
}

MorphologicalAnalysis::MorphologicalAnalysis(const MorphologicalAnalysis & other, Concordance * concordance) :
    mTextFormId(other.mTextFormId), mWritingSystem(other.mWritingSystem), mAllomorphs(other.mAllomorphs), mConcordance(concordance)
{
}

MorphologicalAnalysis::~MorphologicalAnalysis()
{
    qDeleteAll(mAllomorphs);
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
    MorphologicalAnalysis *other = new MorphologicalAnalysis(mTextFormId, mWritingSystem, mConcordance);
    for(int i=0; i<mAllomorphs.count(); i++)
    {
        other->addAllomorph( new Allomorph(mAllomorphs.at(i)->id(), mAllomorphs.at(i)->lexicalEntryId(), mAllomorphs.at(i)->textBit(), mAllomorphs.at(i)->glosses(), mAllomorphs.at(i)->type() ) );
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
        if( ! mAllomorphs.at(i)->equalExceptGuid( other.mAllomorphs.at(i) ) )
        {
            return false;
        }
    }
    return true;
}

TextBit MorphologicalAnalysis::baselineSummary() const
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
    return TextBit( summary, mWritingSystem );
}

TextBit MorphologicalAnalysis::baselineText(int i) const
{
    Q_ASSERT( i < mAllomorphs.count() );
    QString baseline;
    if( i > 0 )
    {
        if( mAllomorphs.at(i)->isClitic() )
            baseline += "=";
        else
            baseline += "-";
    }
    baseline += mAllomorphs.at(i)->text();
    /// NB: the id field of the TextBit contains the index
    return TextBit(baseline, mWritingSystem, i);
}

TextBit MorphologicalAnalysis::gloss(int i, const WritingSystem &ws) const
{
    Q_ASSERT( i < mAllomorphs.count() );
    QString gloss;
    if( i > 0 )
    {
        if( mAllomorphs.at(i)->isClitic() )
            gloss += "=";
        else
            gloss += "-";
    }
    gloss += mAllomorphs.at(i)->gloss(ws).text();
    /// NB: the id field of the TextBit contains the index
    return TextBit(gloss, ws, i);
}

AllomorphPointerIterator MorphologicalAnalysis::allomorphIterator() const
{
    return QListIterator<Allomorph*>(mAllomorphs);
}

void MorphologicalAnalysis::addAllomorph(Allomorph * allomorph)
{
    allomorph->connectToConcordance( mConcordance );
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
    return mAllomorphs[i];
}

int MorphologicalAnalysis::indexFromId(qlonglong id) const
{
    for(int i=0; i<mAllomorphs.count(); i++)
    {
        if( mAllomorphs.at(i)->id() == id )
        {
            return i;
        }
    }
    return -1;
}

Allomorph *MorphologicalAnalysis::allomorphFromId(qlonglong id) const
{
    for(int i=0; i<mAllomorphs.count(); i++)
    {
        if( mAllomorphs.at(i)->id() == id )
        {
            return mAllomorphs[i];
        }
    }
    return 0;
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


QDebug operator<<(QDebug dbg, const MorphologicalAnalysis &key)
{
    dbg.nospace() << "MorphologicalAnalysis(TextForm ID: " << key.textFormId() << ", WS: " << key.writingSystem() << ", Allomorphs(";
    AllomorphPointerIterator iter = key.allomorphIterator();
    while( iter.hasNext() )
    {
        dbg.nospace() << * iter.next() << ", ";
    }
    dbg.nospace() << ")";
    return dbg.maybeSpace();
}
