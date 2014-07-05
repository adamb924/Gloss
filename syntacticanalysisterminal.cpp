#include "syntacticanalysisterminal.h"

#include "allomorph.h"

SyntacticAnalysisTerminal::SyntacticAnalysisTerminal(const Allomorph *allomorph)
    : mAllomorph(allomorph)
{
}

const Allomorph *SyntacticAnalysisTerminal::allomorph() const
{
    return mAllomorph;
}
