#include "annotationtype.h"

AnnotationType::AnnotationType() :
    mWritingSystem(WritingSystem()), mLabel(""), mMark("")
{
}

AnnotationType::AnnotationType(const QString &label, const QString &mark, const WritingSystem &ws, const WritingSystem &headerWs) :
    mWritingSystem(ws), mHeaderWritingSystem(headerWs), mLabel(label), mMark(mark)
{
}

WritingSystem AnnotationType::writingSystem() const
{
    return mWritingSystem;
}

WritingSystem AnnotationType::headerWritingSystem() const
{
    return mHeaderWritingSystem;
}

QString AnnotationType::label() const
{
    return mLabel;
}

QString AnnotationType::mark() const
{
    return mMark;
}
