/*!
  \class Annotation
  \ingroup Data
  \brief This is a data class for representing the annotation on a section of audio file.
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

    qlonglong start() const;
    qlonglong end() const;

    QString summaryString() const;

    bool isNull() const;
    bool isValid() const;

private:
    qlonglong mStartTime, mEndTime;
};

#endif // ANNOTATION_H
