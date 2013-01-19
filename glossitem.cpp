#include "glossitem.h"

#include "textbit.h"
#include "project.h"
#include "databaseadapter.h"
#include "textbit.h"
#include "concordance.h"

#include <QtDebug>
#include <QHash>

GlossItem::GlossItem(const TextBit & baselineBit, Project *project, QObject *parent) : QObject(parent)
{
    mBaselineWritingSystem = baselineBit.writingSystem();
    mTextForms.insert(mBaselineWritingSystem, baselineBit );
    mId = -1;

    mDbAdapter = project->dbAdapter();
    mConcordance = project->concordance();

    guessInterpretation();

    setCandidateNumberFromDatabase();

    updateGlossItemConcordance();
}

GlossItem::GlossItem(const WritingSystem & ws, const TextBitHash & textForms, const TextBitHash & glossForms, qlonglong id, Project *project, QObject *parent) : QObject(parent)
{
    mBaselineWritingSystem = ws;

    mTextForms = textForms;
    mGlosses = glossForms;

    mDbAdapter = project->dbAdapter();
    mConcordance = project->concordance();

    if( mId == -1 )
        guessInterpretation();
    else
        setInterpretation(id, false); // false because we have text forms and glosses from the arguments of the constructor

    setCandidateNumberFromDatabase();

    updateGlossItemConcordance();
}

GlossItem::~GlossItem()
{
}

void GlossItem::resetBaselineText( const TextBit & baselineBit )
{
    mBaselineWritingSystem = baselineBit.writingSystem();

    mTextForms.clear();
    mGlosses.clear();

    mTextForms.insert(mBaselineWritingSystem, baselineBit );
    mId = -1;

    guessInterpretation();

    setCandidateNumberFromDatabase();

    emit fieldsChanged();
}

void GlossItem::setInterpretation(qlonglong id, bool takeFormsFromDatabase)
{
    if( mId != id )
    {
        setCandidateNumber(GlossItem::MultipleOption);

        setApprovalStatus(GlossItem::Approved);

        mId = id;

        if( takeFormsFromDatabase )
        {
            mTextForms.clear();
            mGlosses.clear();

            // doing it in this roundabout way makes sure that the proper signals are emitted
            TextBitHashIterator tfIter(mDbAdapter->guessInterpretationTextForms(mId));
            while(tfIter.hasNext())
            {
                tfIter.next();
                setTextForm( tfIter.value() );
            }

            TextBitHashIterator tfGlosses = mDbAdapter->guessInterpretationGlosses(mId);
            while(tfGlosses.hasNext())
            {
                tfGlosses.next();
                setGloss( tfGlosses.value() );
            }
        }
        else
        {
            loadStringsFromDatabase();
        }

        loadMorphologicalAnalysesFromDatabase();

        emit interpretationIdChanged(mId);
        emit fieldsChanged();
    }
    // TODO possibly update the concordance at this point
}

void GlossItem::setGloss(const TextBit & gloss)
{
    if( mGlosses.value(gloss.writingSystem()) != gloss )
    {
        mDbAdapter->updateInterpretationGloss(gloss);
        mGlosses.insert( gloss.writingSystem(), gloss );
        emit fieldsChanged();
        emit glossChanged(gloss);
    }
}

void GlossItem::setTextForm(const TextBit & textForm)
{
    WritingSystem ws = textForm.writingSystem();
    if( mTextForms.value(ws) != textForm )
    {
        mDbAdapter->updateInterpretationTextForm(textForm);
        mTextForms.insert( ws , textForm );

        mConcordance->updateGlossItemTextFormConcordance( this, textForm.id() );

        MorphologicalAnalysis newMa = mDbAdapter->morphologicalAnalysisFromTextFormId( textForm.id() );
        mMorphologicalAnalyses.insert( ws , newMa );

        emit fieldsChanged();
        emit textFormChanged(textForm);
        emit morphologicalAnalysisChanged( mMorphologicalAnalyses.value( ws ) );
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

const TextBitHash* GlossItem::textForms() const
{
    return &mTextForms;
}

const TextBitHash* GlossItem::glosses() const
{
    return &mGlosses;
}

void GlossItem::setCandidateNumber(CandidateNumber status)
{
    if( mCandidateNumber != status )
    {
        mCandidateNumber = status;
        emit candidateNumberChanged(mCandidateNumber);
        emit candidateNumberChanged( mCandidateNumber, baselineText().id() );
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
    loadMorphologicalAnalysesFromDatabase();
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

MorphologicalAnalysis GlossItem::morphologicalAnalysis(const WritingSystem & ws) const
{
    return mMorphologicalAnalyses.value(ws, MorphologicalAnalysis(mTextForms.value(ws)) );
}

void GlossItem::setMorphologicalAnalysis( const MorphologicalAnalysis & analysis )
{
    if( !mMorphologicalAnalyses.contains(analysis.writingSystem()) || ( mMorphologicalAnalyses.contains(analysis.writingSystem()) && mMorphologicalAnalyses.value( analysis.writingSystem() ) != analysis ) )
    {
        mMorphologicalAnalyses.insert( analysis.writingSystem() , analysis);
        emit morphologicalAnalysisChanged( mMorphologicalAnalyses.value( analysis.writingSystem() ) );
    }
}

void GlossItem::setMorphologicalAnalysisFromDatabase( const WritingSystem & ws )
{
    mMorphologicalAnalyses.insert(ws, mDbAdapter->morphologicalAnalysisFromTextFormId( mTextForms.value(ws).id() ) );
    emit morphologicalAnalysisChanged( mMorphologicalAnalyses.value(ws) );
}

void GlossItem::addAllomorphToAnalysis( const Allomorph & allomorph, const WritingSystem & writingSystem )
{
    mMorphologicalAnalyses[writingSystem].addAllomorph(allomorph);
    emit morphologicalAnalysisChanged( mMorphologicalAnalyses.value(writingSystem) );
}

void GlossItem::loadStringsFromDatabase()
{
    TextBitHashIterator tfIter( mTextForms );
    while(tfIter.hasNext())
    {
        tfIter.next();

        TextBit fromTextForms = tfIter.value();
        TextBit fromDatabase = mDbAdapter->textFormFromId( tfIter.value().id() );

        if( fromDatabase.isNull() )
            fromTextForms.setId( mDbAdapter->newTextForm( mId, tfIter.value() ) );
        else
            fromTextForms.setText( fromDatabase.text() );

        setTextForm( fromTextForms );
    }

    TextBitHashIterator gIter( mGlosses );
    while(gIter.hasNext())
    {
        gIter.next();

        TextBit fromGlosses = gIter.value();
        TextBit fromDatabase = mDbAdapter->glossFromId( gIter.value().id() );

        if( fromDatabase.isNull() )
            fromGlosses.setId( mDbAdapter->newGloss( mId, gIter.value() ) );
        else
            fromGlosses.setText( fromDatabase.text() );

        setGloss( fromGlosses );
    }
}

void GlossItem::loadMorphologicalAnalysesFromDatabase()
{
    mMorphologicalAnalyses.clear();
    TextBitHashIterator tfIter( mTextForms );
    while(tfIter.hasNext())
    {
        tfIter.next();
        mMorphologicalAnalyses.insert( tfIter.key() , mDbAdapter->morphologicalAnalysisFromTextFormId( tfIter.value().id() ) );
    }
}

QList<WritingSystem> GlossItem::morphologicalAnalysisLanguages() const
{
    return mMorphologicalAnalyses.keys();
}

Concordance* GlossItem::concordance()
{
    return mConcordance;
}

void GlossItem::updateGlossItemConcordance()
{
    TextBitHashIterator iter(mTextForms);
    while(iter.hasNext())
    {
        iter.next();
        mConcordance->updateGlossItemTextFormConcordance( this, iter.value().id() );
    }
}
