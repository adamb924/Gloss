#include "worddisplaywidget.h"
#include "textbit.h"
#include "project.h"
#include "lingedit.h"
#include "glossline.h"

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
            break;
        case Project::Gloss:
            connect(edit,SIGNAL(stringChanged(TextBit)), mProject, SLOT(updateInterpretationGloss(TextBit)));
            break;
        case Project::Transcription:
            connect(edit,SIGNAL(stringChanged(TextBit)), mProject, SLOT(updateInterpretationTranscription(TextBit)));
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
                mEdits[i]->setText( mProject->getInterpretationOrthography(mTextBit->id(), mGlossLines.at(i).writingSystem() ) );
                break;
            case Project::Transcription:
                mEdits[i]->setText( mProject->getInterpretationTranscription(mTextBit->id(), mGlossLines.at(i).writingSystem() ) );
                break;
            case Project::Gloss:
                mEdits[i]->setText( mProject->getInterpretationGloss(mTextBit->id(), mGlossLines.at(i).writingSystem() ) );
                break;
            }
        }
    }
}

void WordDisplayWidget::guessInterpretation()
{
    if( mBaselineMode == Project::Orthographic )
    {
        QList<qlonglong> candidates =  mProject->candidateInterpretationsOrthographic( mTextBit->text() );

        if( candidates.length() == 0 )
        {
            mTextBit->setId( mProject->newInterpretationFromOrthography(*mTextBit) );
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
    }
    else
    {
        QList<qlonglong> candidates = mProject->candidateInterpretationsPhonetic( mTextBit->text() );
        if( candidates.length() == 0 )
        {
            mTextBit->setId( mProject->newInterpretationFromPhonetic(*mTextBit)  );
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
    }
}
