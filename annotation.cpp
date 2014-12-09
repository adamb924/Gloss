#include "annotation.h"

Annotation::Annotation()
{
}

Annotation::Annotation(const TextBit &header, const TextBit &annotation) : mHeader(header), mAnnotation(annotation)
{
}

Annotation::Annotation(const TextBit &annotation) : mAnnotation(annotation)
{
}

TextBit Annotation::header() const
{
    return mHeader;
}

TextBit Annotation::annotation() const
{
    return mAnnotation;
}

void Annotation::setWritingSystem(const WritingSystem &ws)
{
    mAnnotation.setWritingSystem(ws);
}

void Annotation::setHeaderWritingSystem(const WritingSystem &ws)
{
    mHeader.setWritingSystem(ws);
}
