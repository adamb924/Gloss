/*!
  \class MorphologicalAnalysis
  \ingroup Data
  \brief This class represents a morphological analysis. Its most important member is a list of Allomorph objects. The class is widely used, but MorphologicalAnalysis objects are in particular members of GlossItem.
*/

#ifndef MORPHOLOGICALANALYSIS_H
#define MORPHOLOGICALANALYSIS_H

#include <QList>

#include "allomorph.h"

typedef QListIterator<Allomorph> AllomorphIterator;

class MorphologicalAnalysis
{
public:
    MorphologicalAnalysis();
    MorphologicalAnalysis(const TextBit & textForm);
    MorphologicalAnalysis(qlonglong textFormId, const WritingSystem & ws);
    MorphologicalAnalysis(const MorphologicalAnalysis & other);

    MorphologicalAnalysis& operator=(const MorphologicalAnalysis & other);
    bool operator!=(const MorphologicalAnalysis & other) const;

    QString baselineSummary() const;
    QString glossSummary(const WritingSystem & ws) const;

    qlonglong textFormId() const;
    void setTextFormId(qlonglong textFormId);
    WritingSystem writingSystem() const;


    // handling allomorphs
    AllomorphIterator allomorphIterator() const;
    void addAllomorph(const Allomorph & allomorph);
    bool isEmpty() const;
    int allomorphCount() const;
    Allomorph* allomorph(int i);
    Allomorph* operator[](int i);

private:
    qlonglong mTextFormId;
    WritingSystem mWritingSystem;
    QList<Allomorph> mAllomorphs;

signals:

public slots:

};

#endif // MORPHOLOGICALANALYSIS_H
