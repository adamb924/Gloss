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

GlossItem::~GlossItem()
{
}

void GlossItem::setInterpretation(qlonglong id, bool takeFormsFromDatabase)
{
    if( mId != id )
    {
        setCandidateStatus(GlossItem::MultipleOption);

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
        mDbAdapter->updateInterpretationGloss(gloss);
        mGlosses.insert( gloss.writingSystem(), gloss );
        emit fieldsChanged();
    }
}

void GlossItem::setTextForm(const TextBit & textForm)
{
    WritingSystem ws = textForm.writingSystem();
    if( mTextForms.value(ws) != textForm )
    {
        mDbAdapter->updateInterpretationTextForm(textForm);
        mTextForms.insert( ws , textForm );
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

WritingSystem GlossItem::baselineWritingSystem() const
{
    return mBaselineWritingSystem;
}

TextBit GlossItem::textForm(const WritingSystem & ws)
{
    if( !mTextForms.contains( ws ) )
    {
        QHash<qlonglong,QString> possible = mDbAdapter->interpretationTextForms(mId, ws.id());
        if( possible.count() > 0 )
        {
            qlonglong id = possible.keys().first();
            setTextForm( TextBit( possible.value(id) , ws , id ));
            if( possible.count() > 1 )
                setCandidateStatus(GlossItem::MultipleOption);
        }
        else
        {
            qlonglong id = mDbAdapter->newTextForm(mId, ws.id());
            setTextForm( mDbAdapter->textFormFromId(id) );
        }
    }
    return mTextForms.value(ws);
}

TextBit GlossItem::gloss(const WritingSystem & ws)
{
    if( !mGlosses.contains( ws ) )
    {
        QHash<qlonglong,QString> possible = mDbAdapter->interpretationGlosses(mId, ws.id());
        if( possible.count() > 0 )
        {
            qlonglong id = possible.keys().first();
            setGloss( TextBit( possible.value(id) , ws, id ) );
            if( possible.count() > 1 )
                setCandidateStatus(GlossItem::MultipleOption);
        }
        else
        {
            qlonglong id = mDbAdapter->newGloss(mId, ws.id());
            setGloss( mDbAdapter->glossFromId(id) );
        }
    }
    return mGlosses.value(ws);
}
