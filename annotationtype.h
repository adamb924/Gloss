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
    AnnotationType(const QString & label, const QString & mark, const WritingSystem & ws, const WritingSystem & headerWs);

    //! \brief Return the WritingSystem of the annotation
    WritingSystem writingSystem() const;

    WritingSystem headerWritingSystem() const;

    //! \brief Return the label (i.e., the text) of the annotation
    QString label() const;

    //! \brief Return the mark of the annotation (e.g., an asterisk)
    QString mark() const;

private:
    WritingSystem mWritingSystem;
    WritingSystem mHeaderWritingSystem;
    QString mLabel;
    QString mMark;
};

#endif // ANNOTATIONTYPE_H
