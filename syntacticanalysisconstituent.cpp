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

QString SyntacticAnalysisConstituent::label() const
{
    return mLabel;
}

const Allomorph *SyntacticAnalysisConstituent::allomorph() const
{
    return 0;
}

QList<SyntacticAnalysisElement *> *SyntacticAnalysisConstituent::elements()
{
    return &mElements;
}
