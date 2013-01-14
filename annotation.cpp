#include "annotation.h"

Annotation::Annotation()
{
    mStartTime = -1;
    mEndTime = -1;
}

Annotation::Annotation(qlonglong startTime, qlonglong endTime)
{
    mStartTime = startTime;
    mEndTime = endTime;
}

qlonglong Annotation::start() const
{
    return mStartTime;
}

qlonglong Annotation::end() const
{
    return mEndTime;
}

bool Annotation::isNull() const
{
    return mStartTime == -1 || mEndTime == -1;
}

bool Annotation::isValid() const
{
    return mStartTime != -1 && mEndTime != -1;
}
