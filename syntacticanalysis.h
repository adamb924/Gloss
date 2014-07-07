#ifndef SYNTACTICANALYSIS_H
#define SYNTACTICANALYSIS_H

#include <QString>
#include <QList>

class SyntacticAnalysisElement;

class SyntacticAnalysis
{
public:
    SyntacticAnalysis(const QString & name);

    void createConstituent(const QString &label, QList<SyntacticAnalysisElement*> elements);

    const QList<SyntacticAnalysisElement*>* elements() const;

private:
    QString mName;
    QList<SyntacticAnalysisElement*> mElements;
};

#endif // SYNTACTICANALYSIS_H
