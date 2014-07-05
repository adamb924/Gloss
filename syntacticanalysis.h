#ifndef SYNTACTICANALYSIS_H
#define SYNTACTICANALYSIS_H

#include <QString>
#include <QList>

class SyntacticAnalysisConstituent;
class SyntacticAnalysisElement;
class SyntacticAnalysisTerminal;

class SyntacticAnalysis
{
public:
    SyntacticAnalysis(const QString & name);

    void createConstituent(const QString &label, QList<SyntacticAnalysisTerminal*> elements);

private:
    QString mName;
    QList<SyntacticAnalysisElement*> mElements;
};

#endif // SYNTACTICANALYSIS_H
