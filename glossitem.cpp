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

    mConcordance->updateGlossItemConcordance( this );

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

void GlossItem::setInterpretation(qlonglong id)
{
    if( mId != id )
    {
        setCandidateNumber(GlossItem::MultipleOption);

        setApprovalStatus(GlossItem::Approved);

        mId = id;

        mConcordance->updateGlossItemConcordance( this );

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

        emit fieldsChanged();
    }
}

void GlossItem::setGloss(const TextBit & gloss)
{
    if( mGlosses.value(gloss.writingSystem()) != gloss )
    {
        mConcordance->removeGlossItemGlossIdPair(this, mGlosses.value(gloss.writingSystem()).id() );

        mDbAdapter->updateGloss(gloss);
        mGlosses.insert( gloss.writingSystem(), gloss );

        setGlossNumberFromDatabase( gloss.writingSystem() );

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

        setTextFormNumberFromDatabase( ws );

        mConcordance->updateGlossItemTextFormConcordance( this, textForm.id() );

        if( textUpdate )
            mDbAdapter->clearMorphologicalAnalysis(textForm.id());

        if( mMorphologicalAnalyses.contains( textForm.writingSystem() ) )
        {
            delete mMorphologicalAnalyses.take( textForm.writingSystem() );
        }

        mMorphologicalAnalyses.insert( ws , mDbAdapter->morphologicalAnalysisFromTextFormId( textForm.id(), mConcordance ) );

        emit fieldsChanged();
        emit textFormChanged(textForm);
        emit morphologicalAnalysisChanged( this, mMorphologicalAnalyses.value( ws ) );
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
        emit candidateNumberChanged( mCandidateNumber, mId );
    }
}

void GlossItem::setCandidateNumberFromDatabase()
{
    if( mDbAdapter->hasMultipleCandidateInterpretations( mTextForms.value(mBaselineWritingSystem) ) )
        setCandidateNumber(GlossItem::MultipleOption);
    else
        setCandidateNumber(GlossItem::SingleOption);
    setTextFormNumberFromDatabase();
    setGlossNumberFromDatabase();
}

void GlossItem::setTextFormNumberFromDatabase()
{
    foreach(WritingSystem ws, mTextForms.keys() )
    {
        setTextFormNumberFromDatabase( ws );
    }
}

void GlossItem::setGlossNumberFromDatabase()
{
    foreach(WritingSystem ws, mGlosses.keys() )
    {
        setGlossNumberFromDatabase( ws );
    }
}

void GlossItem::setTextFormNumberFromDatabase(const WritingSystem &ws)
{
    setMultipleTextFormsAvailable( ws, mDbAdapter->multipleTextFormsAvailable( mId , ws ) );
}

void GlossItem::setGlossNumberFromDatabase(const WritingSystem &ws)
{
    setMultipleGlossesAvailable( ws, mDbAdapter->multipleGlossesAvailable( mId , ws ) );
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

    if( mTextForms.count() > 1 || mGlosses.count() > 0 )
        candidates = mDbAdapter->candidateInterpretations(mTextForms,mGlosses);
    else
        candidates =  mDbAdapter->candidateInterpretations( baselineText() );

    if( candidates.count() == 0 )
    {
        if( mTextForms.count() > 0)
            setInterpretation( mDbAdapter->newInterpretation(mTextForms,mGlosses) ); // true because they'll all be blank
        else
            setInterpretation( mDbAdapter->newInterpretation(baselineText()) ); // true because they'll all be blank
    }
    else
    {
        setInterpretation( candidates.at(0) );  // true because the user hadn't had a chance to specify
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
        mTextForms[ws] = mDbAdapter->interpretationTextForm( mId, ws.id() );
        /// @todo The above line is far more efficient, but does it work as well?
//        setTextForm( mDbAdapter->interpretationTextForm( mId, ws.id() ) );
    }
    return mTextForms.value(ws);
}

TextBit GlossItem::gloss(const WritingSystem & ws)
{
    if( !mGlosses.contains( ws ) )
    {
        mGlosses[ws] = mDbAdapter->interpretationGloss( mId , ws.id() );
        /// @todo The above line is far more efficient, but does it work as well?
//        setGloss( mDbAdapter->interpretationGloss( mId , ws.id() ) );
    }
    return mGlosses.value(ws);
}

MorphologicalAnalysis * GlossItem::morphologicalAnalysis(const WritingSystem & ws)
{
    return mMorphologicalAnalyses.value(ws, new MorphologicalAnalysis(mTextForms.value(ws), mConcordance) );
}

const MorphologicalAnalysis * GlossItem::morphologicalAnalysis(const WritingSystem & ws) const
{
    return mMorphologicalAnalyses.value(ws, new MorphologicalAnalysis(mTextForms.value(ws), mConcordance) );
}

void GlossItem::setMorphologicalAnalysis( MorphologicalAnalysis * analysis )
{
    if( !mMorphologicalAnalyses.contains(analysis->writingSystem()) ||
              !mMorphologicalAnalyses.value( analysis->writingSystem() )->equalExceptGuid( *analysis ) )
    {
        if( mMorphologicalAnalyses.contains(analysis->writingSystem()) )
        {
            delete mMorphologicalAnalyses.take( analysis->writingSystem() );
        }

        mMorphologicalAnalyses.insert( analysis->writingSystem() , analysis );
        emit fieldsChanged();
        MorphologicalAnalysis *ma = mMorphologicalAnalyses.value( analysis->writingSystem() );
        emit morphologicalAnalysisChanged( this, ma );
    }
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
                fromTextForms.setId( mDbAdapter->newTextForm( mId, tfIter.value() ).id() );
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
                fromGlosses.setId( mDbAdapter->newGloss( mId, gIter.value() ).id() );
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
    TextBitHashIterator tfIter( mTextForms );
    while(tfIter.hasNext())
    {
        tfIter.next();
        if( mDbAdapter->textFormHasMorphologicalAnalysis( tfIter.value().id() ) )
        {
            if( mMorphologicalAnalyses.contains( tfIter.value().writingSystem() ) )
            {
                delete mMorphologicalAnalyses.take( tfIter.value().writingSystem() );
            }

            MorphologicalAnalysis * databaseMA = mDbAdapter->morphologicalAnalysisFromTextFormId( tfIter.value().id(), mConcordance );
            mMorphologicalAnalyses.insert( tfIter.key() , databaseMA );
        }
    }
}

bool GlossItem::isPunctuation() const
{
    QString text = baselineText().text();
    for( int i=0; i<text.length(); i++ )
    {
        if( !text.at(i).isPunct() )
        {
            return false;
        }
    }
    return true;
}

bool GlossItem::multipleTextFormsAvailable(const WritingSystem &ws) const
{
    return mMultipleTextForms.value(ws, false);
}

bool GlossItem::multipleGlossesAvailable(const WritingSystem &ws) const
{
    return mMultipleGlosses.value(ws, false);
}

void GlossItem::setMultipleTextFormsAvailable(const WritingSystem &ws, bool multiple)
{
    if( mMultipleTextForms.value(ws) != multiple )
    {
        mMultipleTextForms[ws] = multiple;
        emit textFormNumberChanged(multiple, mId, ws );
    }
}

void GlossItem::setMultipleGlossesAvailable(const WritingSystem &ws, bool multiple)
{
    if( mMultipleGlosses.value(ws) != multiple )
    {
        mMultipleGlosses[ws] = multiple;
        emit glossNumberChanged(multiple, mId, ws );
    }
}

QList<WritingSystem> GlossItem::nonEmptyMorphologicalAnalysisLanguages() const
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
    mConcordance->updateGlossItemConcordance( this );

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
    connect( this, SIGNAL(textFormNumberChanged(bool,qlonglong,const WritingSystem&)), mConcordance, SLOT(updateTextFormNumber(bool,qlonglong,const WritingSystem&)));
    connect( this, SIGNAL(glossNumberChanged(bool,qlonglong,const WritingSystem&)), mConcordance, SLOT(updateGlossNumber(bool,qlonglong,const WritingSystem&)) );
    connect( this, SIGNAL(textFormChanged(TextBit)), mConcordance, SLOT(updateTextForm(TextBit)), Qt::UniqueConnection);
    connect( this, SIGNAL(glossChanged(TextBit)), mConcordance, SLOT(updateGloss(TextBit)), Qt::UniqueConnection);
    connect( this, SIGNAL(morphologicalAnalysisChanged(const GlossItem*,const MorphologicalAnalysis*)), mConcordance, SLOT(updateGlossItemMorphologicalAnalysis(const GlossItem*,const MorphologicalAnalysis*)), Qt::UniqueConnection);
}
