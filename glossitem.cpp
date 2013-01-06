#include "glossitem.h"
#include "textbit.h"
#include "project.h"
#include "databaseadapter.h"

#include <QtDebug>
#include <QHash>
#include "textbit.h"

GlossItem::GlossItem(const TextBit & baselineBit, DatabaseAdapter *dbAdapter, QObject *parent) : QObject(parent)
{
    mBaselineWritingSystem = baselineBit.writingSystem();
    mTextForms.insert(mBaselineWritingSystem, baselineBit );
    mId = -1;

    mDbAdapter = dbAdapter;

    guessInterpretation();

    setCandidateNumberFromDatabase();
}

GlossItem::GlossItem(const WritingSystem & ws, const TextBitHash & textForms, const TextBitHash & glossForms, qlonglong id, DatabaseAdapter *dbAdapter, QObject *parent) : QObject(parent)
{
    mBaselineWritingSystem = ws;

    mTextForms = textForms;
    mGlosses = glossForms;

    mDbAdapter = dbAdapter;

    mId = id;
    if( mId == -1 )
        guessInterpretation();
    else
        setInterpretation(id, false); // false because we have text forms and glosses from the arguments of the constructor

    setCandidateNumberFromDatabase();
}

GlossItem::~GlossItem()
{
}

void GlossItem::setInterpretation(qlonglong id, bool takeFormsFromDatabase)
{
    if( mId != id )
    {
        setCandidateNumber(GlossItem::MultipleOption);

        mId = id;

        if( takeFormsFromDatabase )
        {
            mTextForms.clear();
            mGlosses.clear();
            mTextForms = mDbAdapter->getInterpretationTextForms(mId);
            mGlosses = mDbAdapter->getInterpretationGlosses(mId);
        }

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

void GlossItem::setCandidateNumber(CandidateNumber status)
{
    if( mCandidateNumber != status )
    {
        mCandidateNumber = status;
        emit candidateNumberChanged(mCandidateNumber);
    }
}

void GlossItem::setCandidateNumberFromDatabase()
{
    if( mDbAdapter->hasMultipleCandidateInterpretations( mTextForms.value(mBaselineWritingSystem) ) )
        setCandidateNumber(GlossItem::MultipleOption);
    else
        setCandidateNumber(GlossItem::SingleOption);
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

GlossItem::CandidateNumber GlossItem::candidateNumber() const
{
    return mCandidateNumber;
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
            setInterpretation( mDbAdapter->newInterpretation(mTextForms,mGlosses) , true ); // true because they'll all be blank
        else
            setInterpretation( mDbAdapter->newInterpretation(baselineText()) , true ); // true because they'll all be blank
    }
    else
    {
        setInterpretation( candidates.at(0), true );  // true because the user hadn't had a chance to specify
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
        }
        else
        {
            qlonglong id = mDbAdapter->newGloss(mId, ws.id());
            setGloss( mDbAdapter->glossFromId(id) );
        }
    }
    return mGlosses.value(ws);
}

MorphologicalAnalysis* GlossItem::morphologicalAnalysis(const WritingSystem & ws)
{
    return &mMorphologicalAnalysis[ ws ];
}

void GlossItem::setMorphologicalAnalysis( const WritingSystem & ws, const MorphologicalAnalysis & analysis )
{
    mMorphologicalAnalysis.insert(ws, analysis);
}

void GlossItem::addAllomorphToAnalysis( const Allomorph & allomorph, const WritingSystem & writingSystem )
{
    mMorphologicalAnalysis[writingSystem].append( allomorph );
}
