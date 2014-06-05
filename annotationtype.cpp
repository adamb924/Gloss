#include "annotationtype.h"

AnnotationType::AnnotationType() :
    mWritingSystem(WritingSystem()), mLabel(""), mMark("")
{
}

AnnotationType::AnnotationType(const QString &label, const QString &mark, const WritingSystem &ws) :
    mWritingSystem(ws), mLabel(label), mMark(mark)
{
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
