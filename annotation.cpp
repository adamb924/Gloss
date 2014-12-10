#include "annotation.h"

Annotation::Annotation()
{
}

Annotation::Annotation(const TextBit &header, const TextBit &annotation) : mHeader(header), mText(annotation)
{
}

Annotation::Annotation(const TextBit &annotation) : mText(annotation)
{
}

TextBit Annotation::header() const
{
    return mHeader;
}

TextBit Annotation::text() const
{
    return mText;
}

void Annotation::setWritingSystem(const WritingSystem &ws)
{
    mText.setWritingSystem(ws);
}

void Annotation::setHeaderWritingSystem(const WritingSystem &ws)
{
    mHeader.setWritingSystem(ws);
}
