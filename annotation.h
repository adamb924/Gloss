#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <QtGlobal>
#include <QString>

class Annotation
{
public:
    Annotation();
    Annotation(qint64 startTime, qint64 endTime);

    qlonglong start() const;
    qlonglong end() const;

    bool isNull() const;

private:
    qint64 mStartTime, mEndTime;
    bool mFlag;
    bool mIsNull;
};

#endif // ANNOTATION_H
