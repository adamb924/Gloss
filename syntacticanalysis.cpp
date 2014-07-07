#include "syntacticanalysis.h"

#include "syntacticanalysiselement.h"

SyntacticAnalysis::SyntacticAnalysis(const QString &name)
    : mName(name)
{
}

void SyntacticAnalysis::createConstituent(const QString &label, QList<SyntacticAnalysisElement*> elements)
{
    SyntacticAnalysisElement *tmp = new SyntacticAnalysisElement(label, elements );
    mElements << tmp;
}

const QList<SyntacticAnalysisElement *> *SyntacticAnalysis::elements() const
{
    return &mElements;
}
