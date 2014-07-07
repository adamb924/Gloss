#include "syntacticanalysiselement.h"

SyntacticAnalysisElement::SyntacticAnalysisElement(const Allomorph * allomorph)
    : mAllomorph(allomorph),
      mType(SyntacticAnalysisElement::Terminal)
{
}

SyntacticAnalysisElement::SyntacticAnalysisElement(const QString & label, QList<SyntacticAnalysisElement *> &elements)
    : mLabel(label),
      mType(SyntacticAnalysisElement::Consituent)
{
    for(int i=0; i<elements.count(); i++)
    {
        mElements << elements[i];
    }
}

QString SyntacticAnalysisElement::label() const
{
    return mLabel;
}

const Allomorph *SyntacticAnalysisElement::allomorph() const
{
    return mAllomorph;
}

QList<SyntacticAnalysisElement *> *SyntacticAnalysisElement::elements()
{
    return &mElements;
}

bool SyntacticAnalysisElement::isTerminal() const
{
    return mType == SyntacticAnalysisElement::Terminal;
}
