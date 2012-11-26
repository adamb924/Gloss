#include "worddisplaywidget.h"
#include "textbit.h"
#include "project.h"
#include "lingedit.h"
#include "glossline.h"
#include "databaseadapter.h"

#include <QtGui>
#include <QtDebug>

WordDisplayWidget::WordDisplayWidget( TextBit *bit, Project::BaselineMode baselineMode, Project *project)
{
    mBaselineMode = baselineMode;
    mTextBit = bit;
    mProject = project;

    mGlossLines = mProject->glossLines();

    if( mTextBit->id() == -1 )
        guessInterpretation();

    setupLayout();

    setMinimumSize(128,128);

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));

    fillData();
}

void WordDisplayWidget::setupLayout()
{
    mLayout = new QVBoxLayout;
    setLayout(mLayout);

    mEdits.clear();

    mLayout->addWidget(new QLabel(mTextBit->text()));
    for(int i=0; i<mGlossLines.count(); i++)
    {
        LingEdit *edit = new LingEdit( TextBit( QString("") , mGlossLines.at(i).writingSystem(), mTextBit->id() ), this);
        mLayout->addWidget(edit);
        mEdits << edit;

        switch( mGlossLines.at(i).type() )
        {
        case Project::Orthographic:
            connect(edit, SIGNAL(stringChanged(TextBit)), mProject->dbAdapter(), SLOT(updateInterpretationOrthography(TextBit)));
            connect( edit, SIGNAL(stringChanged(TextBit)), this, SIGNAL(orthographyChanged(TextBit)));
            break;
        case Project::Gloss:
            connect(edit,SIGNAL(stringChanged(TextBit)), mProject->dbAdapter(), SLOT(updateInterpretationGloss(TextBit)));
            connect( edit, SIGNAL(stringChanged(TextBit)), this, SIGNAL(glossChanged(TextBit)));
            break;
        case Project::Transcription:
            connect(edit,SIGNAL(stringChanged(TextBit)), mProject->dbAdapter(), SLOT(updateInterpretationTranscription(TextBit)));
            connect( edit, SIGNAL(stringChanged(TextBit)), this, SIGNAL(transcriptionChanged(TextBit)));
            break;
        }
    }
}


void WordDisplayWidget::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);
    menu.addAction(tr("New gloss..."),this,SLOT(newGloss()));
    menu.exec(event->globalPos());
}

void WordDisplayWidget::newGloss()
{
}

QSize WordDisplayWidget::sizeHint() const
{
    return QSize(128,128);
}

void WordDisplayWidget::fillData()
{
    if( mTextBit->id() != -1 )
    {
        for(int i=0; i<mGlossLines.count();i++)
        {
            switch( mGlossLines.at(i).type() )
            {
            case Project::Orthography:
                mEdits[i]->setText( mProject->dbAdapter()->getInterpretationOrthography(mTextBit->id(), mGlossLines.at(i).writingSystem() ) );
                break;
            case Project::Transcription:
                mEdits[i]->setText( mProject->dbAdapter()->getInterpretationTranscription(mTextBit->id(), mGlossLines.at(i).writingSystem() ) );
                break;
            case Project::Gloss:
                mEdits[i]->setText( mProject->dbAdapter()->getInterpretationGloss(mTextBit->id(), mGlossLines.at(i).writingSystem() ) );
                break;
            }
        }
    }
}

void WordDisplayWidget::guessInterpretation()
{
    if( mBaselineMode == Project::Orthographic )
        guessInterpretationOrthographic();
    else
        guessInterpretationPhonetic();
}

void WordDisplayWidget::guessInterpretationOrthographic()
{
    QList<qlonglong> candidates =  mProject->dbAdapter()->candidateInterpretationsOrthographic( mTextBit->text() );
    qlonglong oldId = mTextBit->id();
    if( candidates.length() == 0 )
    {
        mTextBit->setId( mProject->dbAdapter()->newInterpretationFromOrthography(*mTextBit) );
        mCandidateStatus = SingleOption;
    }
    else if ( candidates.length() == 1 )
    {
        mTextBit->setId( candidates.at(0) );
        mCandidateStatus = SingleOption;
    }
    else // greater than 1
    {
        mTextBit->setId( candidates.at(0) );
        mCandidateStatus = MultipleOption;
    }
    mApprovalStatus = Unapproved;
    emit idChanged(this,oldId,mTextBit->id());
}

void WordDisplayWidget::guessInterpretationPhonetic()
{
    QList<qlonglong> candidates = mProject->dbAdapter()->candidateInterpretationsPhonetic( mTextBit->text() );
    qlonglong oldId = mTextBit->id();
    if( candidates.length() == 0 )
    {
        mTextBit->setId( mProject->dbAdapter()->newInterpretationFromPhonetic(*mTextBit)  );
        mCandidateStatus = SingleOption;
    }
    else if ( candidates.length() == 1 )
    {
        mTextBit->setId( candidates.at(0) );
        mCandidateStatus = SingleOption;
    }
    else // greater than 1
    {
        mTextBit->setId( candidates.at(0) );
        mCandidateStatus = MultipleOption;
    }
    mApprovalStatus = Unapproved;
    emit idChanged(this,oldId,mTextBit->id());
}

TextBit* WordDisplayWidget::textBit() const
{
    return mTextBit;
}

void WordDisplayWidget::updateEdit( const TextBit & bit, Project::GlossLineType type )
{
    LingEdit *line = getAppropriateEdit(bit, type);
    if( line != 0 )
        line->setText(bit.text());
}

LingEdit* WordDisplayWidget::getAppropriateEdit(const TextBit & bit, Project::GlossLineType type )
{
    for(int i=0; i<mGlossLines.count(); i++)
        if( mGlossLines.at(i).type() == type && mGlossLines.at(i).writingSystem()->flexString() == bit.writingSystem()->flexString() )
            return mEdits[i];
    return 0;
}
