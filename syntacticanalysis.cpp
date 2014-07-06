#include "syntacticanalysis.h"

#include "syntacticanalysisconstituent.h"
#include "syntacticanalysisterminal.h"
#include "syntacticanalysiselement.h"

SyntacticAnalysis::SyntacticAnalysis(const QString &name)
    : mName(name)
{
}

void SyntacticAnalysis::createConstituent(const QString &label, QList<SyntacticAnalysisTerminal*> elements)
{

//    mElements.append( new SyntacticAnalysisConstituent(label, elements) );
    SyntacticAnalysisConstituent *tmp = new SyntacticAnalysisConstituent(label, elements );
    mElements << tmp;
}

const QList<SyntacticAnalysisConstituent *> *SyntacticAnalysis::elements() const
{
    return &mElements;
}
