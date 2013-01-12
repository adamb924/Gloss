#include "annotation.h"

Annotation::Annotation()
{
    mIsNull = true;
}

Annotation::Annotation(qint64 startTime, qint64 endTime)
{
    mIsNull = false;
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
    return mIsNull;
}
