#include "morphologicalanalysis.h"
#include "allomorph.h"

#include <QStringList>

MorphologicalAnalysis::MorphologicalAnalysis() :
    QList<Allomorph>()
{
}

QString MorphologicalAnalysis::baselineSummary() const
{
    QString summary;
    for(int i=0; i<count(); i++)
        summary += at(i).text();
    return summary;
}

QString MorphologicalAnalysis::glossSummary(const WritingSystem & ws) const
{
    QStringList summary;
    for(int i=0; i<count(); i++)
    {
        summary << at(i).gloss(ws).text();
    }
    return summary.join("-");
}
