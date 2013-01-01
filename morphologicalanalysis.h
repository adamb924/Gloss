#ifndef MORPHOLOGICALANALYSIS_H
#define MORPHOLOGICALANALYSIS_H

#include <QList>

#include "allomorph.h"

class MorphologicalAnalysis : public QList<Allomorph>
{
public:
    explicit MorphologicalAnalysis();

    QString baselineSummary() const;

signals:

public slots:

};

typedef QListIterator<Allomorph> MorphologicalAnalysisIterator;

#endif // MORPHOLOGICALANALYSIS_H
