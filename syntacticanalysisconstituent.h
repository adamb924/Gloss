#ifndef SYNTACTICANALYSISCONSTITUENT_H
#define SYNTACTICANALYSISCONSTITUENT_H

#include "syntacticanalysiselement.h"
#include <QString>
#include <QList>

class SyntacticAnalysisElement;
class SyntacticAnalysisTerminal;

class SyntacticAnalysisConstituent : public SyntacticAnalysisElement
{
public:
    SyntacticAnalysisConstituent(const QString &label, QList<SyntacticAnalysisElement *> &elements);
    SyntacticAnalysisConstituent(const QString &label, QList<SyntacticAnalysisTerminal *> &elements);

    QString label() const;
    const Allomorph * allomorph() const;
    QList<SyntacticAnalysisElement *>* elements();

private:
    QString mLabel;
    QList<SyntacticAnalysisElement *> mElements;
};

#endif // SYNTACTICANALYSISCONSTITUENT_H
