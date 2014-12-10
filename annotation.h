#ifndef ANNOTATION_H
#define ANNOTATION_H

#include "textbit.h"

class Annotation
{
public:
    Annotation();
    Annotation(const TextBit & header, const TextBit & text);
    Annotation(const TextBit & text);

    TextBit header() const;
    TextBit text() const;
    void setWritingSystem(const WritingSystem & ws);
    void setHeaderWritingSystem(const WritingSystem & ws);

private:
    TextBit mHeader;
    TextBit mText;
};

#endif // ANNOTATION_H
