#include "orthographicworddisplaywidget.h"

#include <QtDebug>

#include "project.h"
#include "textbit.h"

OrthographicWordDisplayWidget::OrthographicWordDisplayWidget(TextBit *bit, Project *project) : AbstractWordDisplayWidget(bit,project)
{
}

void OrthographicWordDisplayWidget::guessInterpretation()
{
    QList<qlonglong> candidates =  mProject->candidateInterpretationsOrthographic( mTextBit->text() );

    qDebug() << "OrthographicWordDisplayWidget::guessInterpretation" << candidates;

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
    fillData();
}
