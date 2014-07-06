#ifndef SYNTACTICANALYSISTERMINAL_H
#define SYNTACTICANALYSISTERMINAL_H

#include "syntacticanalysiselement.h"

class Allomorph;

class SyntacticAnalysisTerminal : public SyntacticAnalysisElement
{
public:
    SyntacticAnalysisTerminal(const Allomorph * allomorph);

    const Allomorph* allomorph() const;

    QString label() const;

private:
    const Allomorph *mAllomorph;
};

#endif // SYNTACTICANALYSISTERMINAL_H
