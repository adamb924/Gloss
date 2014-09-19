#include "syntacticanalysiselementmime.h"

SyntacticAnalysisElementMime::SyntacticAnalysisElementMime(SyntacticAnalysisElement * element)
{
    mElements.append( element );
}

SyntacticAnalysisElementMime::SyntacticAnalysisElementMime(const QList<SyntacticAnalysisElement *> &elements) :
    mElements(elements)
{
}

QList<SyntacticAnalysisElement *> SyntacticAnalysisElementMime::elements() const
{
    return mElements;
}
