#include "glossitem.h"

#include "textbit.h"
#include "project.h"
#include "databaseadapter.h"
#include "textbit.h"
#include "concordance.h"
#include "focus.h"

#include <QtDebug>
#include <QHash>

GlossItem::GlossItem(const TextBit & baselineBit, Project *project, QObject *parent) : QObject(parent),
    mBaselineWritingSystem(baselineBit.writingSystem()), mProject(project), mDbAdapter(mProject->dbAdapter()), mConcordance(mProject->concordance()), mId(-1)
{
    mTextForms.insert(mBaselineWritingSystem, baselineBit );
    guessInterpretation();
    setCandidateNumberFromDatabase();
}

GlossItem::GlossItem(const WritingSystem & ws, const TextBitHash & textForms, const TextBitHash & glossForms, Project *project, QObject *parent) : QObject(parent),
    mBaselineWritingSystem(ws), mProject(project), mDbAdapter(mProject->dbAdapter()), mConcordance(mProject->concordance()), mTextForms(textForms), mGlosses(glossForms)
{
    guessInterpretation();
    setCandidateNumberFromDatabase();
    updateGlossItemConcordance();
}

GlossItem::GlossItem(const WritingSystem & ws, const QSet<qlonglong> & textForms, const QSet<qlonglong> & glossForms, qlonglong interpretationId, Project *project, QObject *parent ) : QObject(parent),
    mBaselineWritingSystem(ws), mProject(project), mDbAdapter(mProject->dbAdapter()), mConcordance(mProject->concordance()), mId(interpretationId)
{
    QSetIterator<qlonglong> tfIter(textForms);
    while(tfIter.hasNext())
    {
        TextBit form = mDbAdapter->textFormFromId( tfIter.next() );
        mTextForms.insert( form.writingSystem() , form );
    }

    QSetIterator<qlonglong> gIter(glossForms);
    while(gIter.hasNext())
    {
        TextBit gloss = mDbAdapter->glossFromId( gIter.next() );
        mGlosses.insert( gloss.writingSystem() , gloss );
    }

    setCandidateNumberFromDatabase();
    updateGlossItemConcordance();
}

GlossItem::~GlossItem()
{
    qDeleteAll(mMorphologicalAnalyses);
}

void GlossItem::resetBaselineText( const TextBit & baselineBit )
{
    mBaselineWritingSystem = baselineBit.writingSystem();

    mTextForms.clear();
    mGlosses.clear();

    mTextForms.insert(mBaselineWritingSystem, baselineBit );
    mId = -1;

    guessInterpretation();

    updateGlossItemConcordance();

    setCandidateNumberFromDatabase();

    emit fieldsChanged();
    emit baselineTextChanged(mTextForms.value(mBaselineWritingSystem));
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

        emit fieldsChanged();
    }
    // TODO possibly update the concordance at this point
}

void GlossItem::setGloss(const TextBit & gloss)
{
    if( mGlosses.value(gloss.writingSystem()) != gloss )
    {
        mConcordance->removeGlossItemGlossIdPair(this, mGlosses.value(gloss.writingSystem()).id() );

        mDbAdapter->updateGloss(gloss);
        mGlosses.insert( gloss.writingSystem(), gloss );

        mConcordance->updateGlossItemGlossConcordance( this, gloss.id() );

        emit fieldsChanged();
        emit glossChanged(gloss);
    }
}

void GlossItem::setGlossText(const TextBit & gloss)
{
    if( mGlosses.value(gloss.writingSystem()).id() == gloss.id() )
    {
        if( mGlosses.value(gloss.writingSystem()).text() != gloss.text() )
        {
            mGlosses[gloss.writingSystem()].setText( gloss.text() );
            emit glossChanged(gloss);
            emit fieldsChanged();
        }
    }
}

void GlossItem::setTextForm(const TextBit & textForm)
{
    WritingSystem ws = textForm.writingSystem();

    bool textUpdate = mTextForms.value(ws).id() == textForm.id() && mTextForms.value(ws).text() != textForm.text();

    if( mTextForms.value(ws) != textForm )
    {
        mConcordance->removeGlossItemTextFormIdPair( this, mTextForms.value(ws).id() );

        mDbAdapter->updateTextForm(textForm);
        mTextForms.insert( ws , textForm );

        mConcordance->updateGlossItemTextFormConcordance( this, textForm.id() );

        if( textUpdate )
            mDbAdapter->clearMorphologicalAnalysis(textForm.id());

        mMorphologicalAnalyses.insert( ws , mDbAdapter->morphologicalAnalysisFromTextFormId( textForm.id() ) );

        emit fieldsChanged();
        emit textFormChanged(textForm);
        emit morphologicalAnalysisChanged( mMorphologicalAnalyses.value( ws ) );
    }
}

void GlossItem::setTextFormText(const TextBit & textForm)
{
    if( mTextForms.value(textForm.writingSystem()).id() == textForm.id() )
    {
        if( mTextForms.value(textForm.writingSystem()).text() != textForm.text() )
        {
            mTextForms[textForm.writingSystem()].setText(textForm.text());
            mDbAdapter->clearMorphologicalAnalysis( textForm.id() );
            emit textFormChanged(textForm);
            emit fieldsChanged();
        }
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

MorphologicalAnalysis * GlossItem::morphologicalAnalysis(const WritingSystem & ws)
{
    if( !mMorphologicalAnalyses.contains(ws) && mDbAdapter->hasMorphologicalAnalysis( mTextForms.value(ws).id() ) )
        mMorphologicalAnalyses.insert( ws , mDbAdapter->morphologicalAnalysisFromTextFormId( mTextForms.value(ws).id() ) );

    return mMorphologicalAnalyses.value(ws, new MorphologicalAnalysis(mTextForms.value(ws)) );
}

void GlossItem::setMorphologicalAnalysis( MorphologicalAnalysis * analysis )
{
    if( !mMorphologicalAnalyses.contains(analysis->writingSystem()) || ( mMorphologicalAnalyses.contains(analysis->writingSystem()) && *mMorphologicalAnalyses.value( analysis->writingSystem() ) != *analysis ) )
    {
        mMorphologicalAnalyses.insert( analysis->writingSystem() , new MorphologicalAnalysis(*analysis) );
        emit fieldsChanged();
        emit morphologicalAnalysisChanged( mMorphologicalAnalyses.value( analysis->writingSystem() ) );
    }
}

void GlossItem::setMorphologicalAnalysisFromDatabase( const WritingSystem & ws )
{
    mMorphologicalAnalyses.insert(ws, mDbAdapter->morphologicalAnalysisFromTextFormId( mTextForms.value(ws).id() ) );
    emit morphologicalAnalysisChanged( mMorphologicalAnalyses.value(ws) );
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
        {
            QHash<qlonglong,QString> textForms = mDbAdapter->interpretationTextForms( mId, tfIter.key().id() );
            if(textForms.count() > 0 )
                fromTextForms.setId( textForms.keys().first() );
            else
                fromTextForms.setId( mDbAdapter->newTextForm( mId, tfIter.value() ) );
        }
        else
        {
            fromTextForms.setText( fromDatabase.text() );
        }

        setTextForm( fromTextForms );
    }

    TextBitHashIterator gIter( mGlosses );
    while(gIter.hasNext())
    {
        gIter.next();

        TextBit fromGlosses = gIter.value();
        TextBit fromDatabase = mDbAdapter->glossFromId( gIter.value().id() );

        if( fromDatabase.isNull() )
        {
            QHash<qlonglong,QString> glosses = mDbAdapter->interpretationGlosses( mId , gIter.key().id() );
            if(glosses.count() > 0 )
                fromGlosses.setId( glosses.keys().first() );
            else
                fromGlosses.setId( mDbAdapter->newGloss( mId, gIter.value() ) );
        }
        else
        {
            fromGlosses.setText( fromDatabase.text() );
        }

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
        if( mDbAdapter->textFormHasMorphologicalAnalysis( tfIter.value().id() ) )
        {
            mMorphologicalAnalyses.insert( tfIter.key() , mDbAdapter->morphologicalAnalysisFromTextFormId( tfIter.value().id() ) );
            emit morphologicalAnalysisChanged( mMorphologicalAnalyses.value(tfIter.key()) );
        }
    }
}

QList<WritingSystem> GlossItem::morphologicalAnalysisLanguages() const
{
    QList<WritingSystem> languages;
    QHashIterator<WritingSystem,MorphologicalAnalysis*> iter( mMorphologicalAnalyses );

    while(iter.hasNext())
    {
        iter.next();
        if( !iter.value()->isEmpty() )
            languages << iter.key();
    }
    return languages;
}

Concordance* GlossItem::concordance()
{
    return mConcordance;
}

void GlossItem::updateGlossItemConcordance()
{
    mConcordance->removeGlossItemFromConcordance(this);
    TextBitHashIterator iter(mTextForms);
    while(iter.hasNext())
    {
        iter.next();
        mConcordance->updateGlossItemTextFormConcordance( this, iter.value().id() );
    }

    iter = TextBitHashIterator(mGlosses);
    while(iter.hasNext())
    {
        iter.next();
        mConcordance->updateGlossItemGlossConcordance( this, iter.value().id() );
    }
}

Project* GlossItem::project()
{
    return mProject;
}

bool GlossItem::matchesFocus( const Focus & focus ) const
{
    if( focus.type() == Focus::Interpretation )
    {
        return mId == focus.index();
    }
    else if ( focus.type() == Focus::Gloss )
    {
        TextBitHashIterator iter(mGlosses);
        while(iter.hasNext())
        {
            iter.next();
            if( iter.value().id() == focus.index() )
                return true;
        }
    }
    else if ( focus.type() == Focus::TextForm )
    {
        TextBitHashIterator iter(mTextForms);
        while(iter.hasNext())
        {
            iter.next();
            if( iter.value().id() == focus.index() )
                return true;
        }
    }
    else if ( focus.type() == Focus::GlossItem )
    {
        return (qlonglong)this == focus.index();
    }
    return false;
}

TextBit GlossItem::getAnnotation( const QString & key ) const
{
    return mAnnotations.value(key, TextBit() );
}

void GlossItem::setAnnotation( const QString & key, const TextBit & annotation )
{
    mAnnotations.insert(key, annotation);
    // this is sort of a cheap way to make sure the text is marked as changed
    emit approvalStatusChanged(mApprovalStatus);
}

QHashIterator<QString,TextBit> GlossItem::annotations() const
{
    return QHashIterator<QString,TextBit>(mAnnotations);
}

bool GlossItem::hasAnnotations() const
{
    return mAnnotations.count() > 0;
}

bool GlossItem::hasAnnotation( const QString & key ) const
{
    if( mAnnotations.contains( key ) )
        return ! mAnnotations.value(key).text().isEmpty();
    else
        return false;
}

void GlossItem::connectToConcordance()
{
    connect( this, SIGNAL(destroyed(QObject*)), mConcordance, SLOT(removeGlossItemFromConcordance(QObject*)), Qt::UniqueConnection);
    connect( this, SIGNAL(candidateNumberChanged(GlossItem::CandidateNumber,qlonglong)), mConcordance, SLOT(updateInterpretationsAvailableForGlossItem(GlossItem::CandidateNumber,qlonglong)), Qt::UniqueConnection);
    connect( this, SIGNAL(textFormChanged(TextBit)), mConcordance, SLOT(updateTextForm(TextBit)), Qt::UniqueConnection);
    connect( this, SIGNAL(glossChanged(TextBit)), mConcordance, SLOT(updateGloss(TextBit)), Qt::UniqueConnection);
    connect( this, SIGNAL(morphologicalAnalysisChanged(MorphologicalAnalysis*)), mConcordance, SLOT(updateGlossItemMorphologicalAnalysis(MorphologicalAnalysis*)), Qt::UniqueConnection);
}
