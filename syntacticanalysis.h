#ifndef SYNTACTICANALYSIS_H
#define SYNTACTICANALYSIS_H

#include <QString>
#include <QList>

#include "syntacticanalysiselement.h"

class SyntacticAnalysis
{
public:
    SyntacticAnalysis(const QString & name);

private:
    QString mName;
    QList<SyntacticAnalysisElement> mElements;
};

#endif // SYNTACTICANALYSIS_H
