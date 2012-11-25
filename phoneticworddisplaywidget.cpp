#include "phoneticworddisplaywidget.h"

#include "project.h"
#include "textbit.h"

PhoneticWordDisplayWidget::PhoneticWordDisplayWidget(TextBit *bit, Project *project) : AbstractWordDisplayWidget(bit,project)
{
}

void PhoneticWordDisplayWidget::guessInterpretation()
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
