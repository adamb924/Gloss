/*!
  \class AnnotationType
  \ingroup Data
  \brief This class describes a type of annotation to a text (i.e., a note to the text, not an audio file annotation).
*/

#ifndef ANNOTATIONTYPE_H
#define ANNOTATIONTYPE_H

#include "writingsystem.h"

class AnnotationType
{
public:
    AnnotationType();
    AnnotationType(const QString & label, const QString & mark, const WritingSystem & ws);

    WritingSystem writingSystem() const;
    QString label() const;
    QString mark() const;

private:
    WritingSystem mWritingSystem;
    QString mLabel;
    QString mMark;
};

#endif // ANNOTATIONTYPE_H
