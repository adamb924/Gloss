#include "interval.h"

Interval::Interval()
{
    mStartTime = -1;
    mEndTime = -1;
}

Interval::Interval(qlonglong startTime, qlonglong endTime)
{
    mStartTime = startTime;
    mEndTime = endTime;
}

qlonglong Interval::start() const
{
    return mStartTime;
}

qlonglong Interval::end() const
{
    return mEndTime;
}

bool Interval::isNull() const
{
    return mStartTime == -1 || mEndTime == -1;
}

bool Interval::isValid() const
{
    return mStartTime != -1 && mEndTime != -1;
}

QString Interval::summaryString() const
{
    if( isValid() )
        return QString("%1-%2").arg( (float)mStartTime/1000.0f ).arg( (float)mEndTime/1000.0f );
    else
        return QString();
}
