#ifndef MORPHOLOGICALANALYSIS_H
#define MORPHOLOGICALANALYSIS_H

#include <QList>

#include "allomorph.h"

class MorphologicalAnalysis : public QList<Allomorph>
{
public:
    explicit MorphologicalAnalysis();

    QString baselineSummary() const;
    QString glossSummary(const WritingSystem & ws) const;

private:
    qlonglong mId;

signals:

public slots:

};

typedef QListIterator<Allomorph> MorphologicalAnalysisIterator;

#endif // MORPHOLOGICALANALYSIS_H
