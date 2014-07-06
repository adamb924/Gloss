#ifndef SYNTACTICANALYSISELEMENT_H
#define SYNTACTICANALYSISELEMENT_H

class Allomorph;
class QString;

class SyntacticAnalysisElement
{
public:
    SyntacticAnalysisElement();

    virtual QString label() const = 0;
    virtual const Allomorph * allomorph() const = 0;
};

#endif // SYNTACTICANALYSISELEMENT_H
