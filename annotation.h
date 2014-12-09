#ifndef ANNOTATION_H
#define ANNOTATION_H

#include "textbit.h"

class Annotation
{
public:
    Annotation();
    Annotation(const TextBit & header, const TextBit & annotation);
    Annotation(const TextBit & annotation);

    TextBit header() const;
    TextBit annotation() const;
    void setWritingSystem(const WritingSystem & ws);
    void setHeaderWritingSystem(const WritingSystem & ws);

private:
    TextBit mHeader;
    TextBit mAnnotation;
};

#endif // ANNOTATION_H
