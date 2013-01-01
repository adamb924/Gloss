#include "morphologicalanalysis.h"
#include "allomorph.h"

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
