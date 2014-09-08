#include "syntacticanalysiselementmime.h"

SyntacticAnalysisElementMime::SyntacticAnalysisElementMime(SyntacticAnalysisElement * element) :
    mElement(element)
{
}

SyntacticAnalysisElement *SyntacticAnalysisElementMime::element() const
{
    return mElement;
}
