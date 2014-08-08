#ifndef SYNTACTICANALYSIS_H
#define SYNTACTICANALYSIS_H

#include <QString>
#include <QList>
#include <QHash>

class SyntacticAnalysisElement;
class Text;
class Tab;
class Allomorph;

class SyntacticAnalysis
{
public:
    SyntacticAnalysis(const QString & name);

    void createConstituent(const QString &label, QList<SyntacticAnalysisElement*> elements);

    const QList<SyntacticAnalysisElement*>* elements() const;

    void addBaselineElement(SyntacticAnalysisElement * element);

    const QHash<const Allomorph *, SyntacticAnalysisElement *> *allomorphConcordance() const;

    QString name() const;

    void debug() const;

    bool isEmpty() const;

private:

    //! \brief Returns true if all of the elements are terminal elements, otherwise returns false
    bool allTerminals(QList<SyntacticAnalysisElement *> elements) const;

    //! \brief Returns true if any of the elements are already parsed into constituents, otherwise returns false
    bool anyHaveParents(QList<SyntacticAnalysisElement *> elements) const;

    //! \brief Returns true if none of the elements are already parsed into constituents, otherwise returns false
    bool noneHaveParents(QList<SyntacticAnalysisElement *> elements) const;

    //! \brief Returns true if the elements are all sisters, otherwise returns false. Returns true if there are fewer than two elements
    bool areSisters(QList<SyntacticAnalysisElement *> elements);

    //! \brief Returns the address of the object if it has \a element as a child, or tries to find it in its descendants. If no parent is found, returns 0.
    SyntacticAnalysisElement *findParent(SyntacticAnalysisElement *element );

private:
    QString mName;
    QList<SyntacticAnalysisElement*> mElements;
    QHash<const Allomorph*, SyntacticAnalysisElement*> mBaselineConcordance;
};

#endif // SYNTACTICANALYSIS_H
