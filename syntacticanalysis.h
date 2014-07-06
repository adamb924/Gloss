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

    const QList<SyntacticAnalysisConstituent*>* elements() const;

private:
    QString mName;
    QList<SyntacticAnalysisConstituent*> mElements;
};

#endif // SYNTACTICANALYSIS_H
