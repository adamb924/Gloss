#include "annotationtype.h"

AnnotationType::AnnotationType()
{
    mLabel = "";
    mMark = "";
    mWritingSystem = WritingSystem();
}

AnnotationType::AnnotationType(const QString &label, const QString &mark, const WritingSystem &ws)
{
    mLabel = label;
    mMark = mark;
    mWritingSystem = ws;
}

WritingSystem AnnotationType::writingSystem() const
{
    return mWritingSystem;
}

QString AnnotationType::label() const
{
    return mLabel;
}

QString AnnotationType::mark() const
{
    return mMark;
}
