/*!
  \class Annotation
  \ingroup Data
  \brief This is a data class for representing the annotation on a section of audio file. Annotations are recorded with millisecond precision.
*/

#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <QtGlobal>
#include <QString>

class Annotation
{
public:
    Annotation();
    Annotation(qlonglong startTime, qlonglong endTime);

    //! \brief Return the start of the interval in milliseconds.
    qlonglong start() const;

    //! \brief Return the end of the interval in milliseconds.
    qlonglong end() const;

    //! \brief Return a string summarizing the interval.
    QString summaryString() const;

    //! \brief Returns true if the annotation is null, otherwise false.
    bool isNull() const;

    //! \brief Returns true if the annotation is valid, otherwise false.
    bool isValid() const;

private:
    qlonglong mStartTime, mEndTime;
};

#endif // ANNOTATION_H
