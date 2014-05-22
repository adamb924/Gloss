/*!
  \class Interval
  \ingroup Data
  \brief This is a data class for representing an interval of an audio file. Intervals are recorded with millisecond precision.
*/

#ifndef INTERVAL_H
#define INTERVAL_H

#include <QtGlobal>
#include <QString>

class Interval
{
public:
    Interval();
    Interval(qlonglong startTime, qlonglong endTime);

    //! \brief Return the start of the interval in milliseconds.
    qlonglong start() const;

    //! \brief Return the end of the interval in milliseconds.
    qlonglong end() const;

    //! \brief Return a string summarizing the interval.
    QString summaryString() const;

    //! \brief Returns true if the interval is null, otherwise false.
    bool isNull() const;

    //! \brief Returns true if the interval is valid, otherwise false.
    bool isValid() const;

private:
    qlonglong mStartTime, mEndTime;
};

#endif // INTERVAL_H
