#include "glossitem.h"
#include "textbit.h"
#include "project.h"
#include "databaseadapter.h"

#include <QtDebug>

GlossItem::GlossItem(const TextBit & baselineText, Project *project, QObject *parent) : QObject(parent)
{
    mTextItems.insert(baselineText.writingSystem(), baselineText.text() );
    mBaselineWritingSystem = baselineText.writingSystem();
    mId = -1;

    mProject = project;

    guessInterpretation();
}

GlossItem::GlossItem(const WritingSystem & ws, const TextBitHash & textForms, const TextBitHash & glossForms, Project *project, QObject *parent) : QObject(parent)
{
    mBaselineWritingSystem = ws;

    mTextItems = textForms;
    mGlossItems = glossForms;

    mProject = project;

    guessInterpretation();
}

GlossItem::GlossItem(const WritingSystem & ws, qlonglong id, Project *project, QObject *parent)
{
    mProject = project;
    mBaselineWritingSystem = ws;
    setInterpretation(id);
}

void GlossItem::setInterpretation(qlonglong id)
{
    mId = id;
    mTextItems.clear();
    mGlossItems.clear();

    mTextItems = mProject->dbAdapter()->getInterpretationTextForms(mId);
    mGlossItems = mProject->dbAdapter()->getInterpretationGlosses(mId);

    mApprovalStatus = GlossItem::Approved;
}

qlonglong GlossItem::id() const
{
    return mId;
}

TextBit GlossItem::baselineText() const
{
    return TextBit( mTextItems.value(mBaselineWritingSystem) , mBaselineWritingSystem );
}

TextBitHash* GlossItem::textItems()
{
    return &mTextItems;
}

TextBitHash* GlossItem::glossItems()
{
    return &mGlossItems;
}

void GlossItem::setCandidateStatus(CandidateStatus status)
{
    if( mCandidateStatus != status )
    {
        mCandidateStatus = status;
        emit candidateStatusChanged(mCandidateStatus);
    }
}

void GlossItem::setApprovalStatus(ApprovalStatus status)
{
    if( mApprovalStatus != status )
    {
        mApprovalStatus = status;
        emit approvalStatusChanged(mApprovalStatus);
    }
}

GlossItem::ApprovalStatus GlossItem::approvalStatus() const
{
    return mApprovalStatus;
}

void GlossItem::updateGloss( const TextBit & bit )
{
    mGlossItems.insert(bit.writingSystem(), bit.text());
}

void GlossItem::updateText( const TextBit & bit )
{
    mTextItems.insert(bit.writingSystem(), bit.text());
}

void GlossItem::guessInterpretation()
{
    QList<qlonglong> candidates;
    if( mTextItems.count() > 0 || mGlossItems.count() > 0 )
        candidates = mProject->dbAdapter()->candidateInterpretations(mTextItems,mGlossItems);
    else
        candidates =  mProject->dbAdapter()->candidateInterpretations( baselineText() );

    if( candidates.length() == 0 )
    {
        if( mTextItems.count() > 0)
            mProject->dbAdapter()->newInterpretation(mTextItems,mGlossItems);
        else
            setInterpretation( mProject->dbAdapter()->newInterpretation(baselineText()) );
        setCandidateStatus(GlossItem::SingleOption);
    }
    else if ( candidates.length() == 1 )
    {
        setInterpretation( candidates.at(0) );
        setCandidateStatus(GlossItem::SingleOption);
    }
    else // greater than 1
    {
        setInterpretation( candidates.at(0) );
        setCandidateStatus(GlossItem::MultipleOption);
    }
    setApprovalStatus(GlossItem::Unapproved);
}

