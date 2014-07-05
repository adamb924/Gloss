#include "syntacticanalysisconstituent.h"
#include "syntacticanalysiselement.h"
#include "syntacticanalysisterminal.h"

SyntacticAnalysisConstituent::SyntacticAnalysisConstituent(const QString & label, QList<SyntacticAnalysisElement *> &elements)
    : mLabel(label), mElements(elements)
{
}

SyntacticAnalysisConstituent::SyntacticAnalysisConstituent(const QString & label, QList<SyntacticAnalysisTerminal *> &elements)
    : mLabel(label)
{
    for(int i=0; i<elements.count(); i++)
    {
        mElements << elements[i];
    }
}
