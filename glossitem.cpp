#include "glossitem.h"
#include "textbit.h"
#include "project.h"
#include "databaseadapter.h"

#include <QtDebug>

GlossItem::GlossItem(const TextBit & baselineText, DatabaseAdapter *dbAdapter, QObject *parent) : QObject(parent)
{
    mBaselineWritingSystem = baselineText.writingSystem();
    mTextForms.insert(mBaselineWritingSystem, baselineText );
    mId = -1;

    mDbAdapter = dbAdapter;

    qDebug() << baselineText.text() << baselineText.writingSystem().flexString();

    guessInterpretation();
}

GlossItem::GlossItem(const WritingSystem & ws, const TextBitHash & textForms, const TextBitHash & glossForms, DatabaseAdapter *dbAdapter, QObject *parent) : QObject(parent)
{
    mBaselineWritingSystem = ws;

    mTextForms = textForms;
    mGlosses = glossForms;

    mDbAdapter = dbAdapter;

    guessInterpretation();
}

GlossItem::GlossItem(const WritingSystem & ws, qlonglong id, DatabaseAdapter *dbAdapter, QObject *parent)
{
    mDbAdapter = dbAdapter;
    mBaselineWritingSystem = ws;
    setInterpretation(id);
    setApprovalStatus(GlossItem::Approved);
}

void GlossItem::setInterpretation(qlonglong id)
{
    if( mId != id )
    {
        mId = id;
        mTextForms.clear();
        mGlosses.clear();

        mTextForms = mDbAdapter->getInterpretationTextForms(mId);
        mGlosses = mDbAdapter->getInterpretationGlosses(mId);

        emit interpretationIdChanged(mId);
        emit fieldsChanged();
    }
}

void GlossItem::setGloss(const TextBit & gloss)
{
    if( mGlosses.value(gloss.writingSystem()) != gloss )
    {
        mGlosses.insert( gloss.writingSystem(), gloss );
        emit fieldsChanged();
    }
}

void GlossItem::setTextForm(const TextBit & textForm)
{
    if( mTextForms.value(textForm.writingSystem()) != textForm )
    {
        qDebug() << "GlossItem::setTextForm inserting";
        mTextForms.insert( textForm.writingSystem() , textForm );
        if( mTextForms.contains( textForm.writingSystem() )  )
        {
            qDebug() << "mTextForms contains WS" << textForm.writingSystem().id();
        }
        else
        {
            qDebug() << "mTextForms does not contain WS" << textForm.writingSystem().id();
        }
        qDebug() << "now contains is: " << mTextForms.contains( textForm.writingSystem() );
        emit fieldsChanged();
    }
}

qlonglong GlossItem::id() const
{
    return mId;
}

TextBit GlossItem::baselineText() const
{
    return mTextForms.value(mBaselineWritingSystem);
}

TextBitHash* GlossItem::textForms()
{
    return &mTextForms;
}

TextBitHash* GlossItem::glosses()
{
    return &mGlosses;
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

GlossItem::CandidateStatus GlossItem::candidateStatus() const
{
    return mCandidateStatus;
}

void GlossItem::updateGloss( const TextBit & bit )
{
    mDbAdapter->updateInterpretationGloss(bit);
    mGlosses.insert(bit.writingSystem(), bit );
}

void GlossItem::updateText( const TextBit & bit )
{
    mDbAdapter->updateInterpretationTextForm(bit);
    mTextForms.insert(bit.writingSystem(), bit );
}

void GlossItem::guessInterpretation()
{
    QList<qlonglong> candidates;
    if( mTextForms.count() > 0 || mGlosses.count() > 0 )
        candidates = mDbAdapter->candidateInterpretations(mTextForms,mGlosses);
    else
        candidates =  mDbAdapter->candidateInterpretations( baselineText() );

    if( candidates.length() == 0 )
    {
        if( mTextForms.count() > 0)
            setInterpretation( mDbAdapter->newInterpretation(mTextForms,mGlosses) );
        else
            setInterpretation( mDbAdapter->newInterpretation(baselineText()) );
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

void GlossItem::toggleApproval()
{
    if( mApprovalStatus == GlossItem::Approved )
        setApprovalStatus(GlossItem::Unapproved);
    else
        setApprovalStatus(GlossItem::Approved);
}

WritingSystem GlossItem::writingSystem() const
{
    return mBaselineWritingSystem;
}

TextBit GlossItem::textForm(const WritingSystem & ws)
{
    qDebug() << "GlossItem::textForm(const WritingSystem & ws)" << mId << this;
    if( !mTextForms.contains( ws ) )
    {
        qDebug() << "GlossItem::textForm  mTextForms does not contain WS" << ws.id();
        qlonglong id = mDbAdapter->newTextForm( mId , ws.id() );
        setTextForm( TextBit("", ws, id) );
    }
    return mTextForms.value(ws);
}

TextBit GlossItem::gloss(const WritingSystem & ws)
{
    qDebug() << "GlossItem::gloss(const WritingSystem & ws)" << mId << this;
    if( !mGlosses.contains( ws ) )
    {
        qDebug() << "GlossItem::gloss mTextForms does not contain WS" << ws.id();
        qlonglong id = mDbAdapter->newGloss( mId , ws.id() );
        setGloss( TextBit("", ws, id) );
    }
    return mGlosses.value(ws);
}
