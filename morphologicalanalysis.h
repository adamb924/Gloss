/*!
  \class MorphologicalAnalysis
  \ingroup Data
  \brief This class represents a morphological analysis. Its most important member is a list of Allomorph objects. The class is widely used, but MorphologicalAnalysis objects are in particular members of GlossItem.

  An object of MorphologicalAnalysis corresponds to the rows of the SQL table MorphologicalAnalysisMembers, for a given TextFormId.
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

    //! \brief Returns a string representation of the analysis, with the baseline text delimited by morpheme breaks
    QString baselineSummary() const;

    //! \brief Returns a string representation of the analysis, with the gloss text for WritingSystem \a ws delimited by morpheme breaks
    QString glossSummary(const WritingSystem & ws) const;

    //! \brief Returns the text form id for the analysis
    qlonglong textFormId() const;

    //! \brief Set the text form id for the analysis
    void setTextFormId(qlonglong textFormId);

    //! \brief Returns the WritingSystem for the text form of the analysis
    WritingSystem writingSystem() const;

    //! \brief Returns an AllomorphIterator for iterating over the allomorphs of the analysis
    AllomorphIterator allomorphIterator() const;

    //! \brief Add an allomorph to the end of the morphological analysis
    void addAllomorph(const Allomorph & allomorph);

    //! \brief Returns true if the analysis has no allomorphs, otherwise returns false
    bool isEmpty() const;

    //! \brief Returns the number of allomorphs in the analysis
    int allomorphCount() const;

    //! \brief Returns the \a i th allomorph of the analysis
    Allomorph* allomorph(int i);

    //! \brief Returns the \a i th allomorph of the analysis
    Allomorph* operator[](int i);

private:
    qlonglong mTextFormId;
    WritingSystem mWritingSystem;
    QList<Allomorph> mAllomorphs;

};

#endif // MORPHOLOGICALANALYSIS_H
