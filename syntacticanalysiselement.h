/*!
  \class SyntacticAnalysisElement
  \ingroup Data
  \brief A data class representing a syntactic analysis constituent, which either has daughter elements, or is terminal.

    If the element is a terminal element, then allomorph() returns a pointer to the corresponding Allomorph object.

    If the element is a constituent element, then mSyntacticType (SyntacticType) contains information about the element, and elements() returns a list of daugher SyntacticAnalysisElement objects.

    A SyntacticAnalysis object contains a list of SyntacticAnalysisElement objects.
*/

#ifndef SYNTACTICANALYSISELEMENT_H
#define SYNTACTICANALYSISELEMENT_H

class Allomorph;
class QString;
class SyntacticAnalysisElement;

#include "syntactictype.h"

#include <QList>
#include <QString>

class SyntacticAnalysisElement
{
public:
    enum Type { Consituent , Terminal };

    SyntacticAnalysisElement(const Allomorph * allomorph);
    SyntacticAnalysisElement(const SyntacticType &type, const QList<SyntacticAnalysisElement *> & elements);
    ~SyntacticAnalysisElement();

    QString label() const;
    const Allomorph * allomorph() const;
    QList<SyntacticAnalysisElement *> *elements();
    const QList<SyntacticAnalysisElement *> *elements() const;
    SyntacticAnalysisElement *parent();
    bool isTerminal() const;
    bool isConstituent() const;
    void removeChild(SyntacticAnalysisElement * element);
    bool hasChild(SyntacticAnalysisElement *element ) const;
    void addChild(SyntacticAnalysisElement *element);
    void debug() const;
    void setParent(SyntacticAnalysisElement * parent);
    void replaceWithConstituent(const SyntacticType &type, QList<SyntacticAnalysisElement *> &elements);

private:
    const Allomorph *mAllomorph;
    SyntacticType mSyntacticType;
    SyntacticAnalysisElement::Type mType;
    QList<SyntacticAnalysisElement *> mElements;
    SyntacticAnalysisElement * mParent;
};

QDebug operator<<(QDebug dbg, const SyntacticAnalysisElement &key);

#endif // SYNTACTICANALYSISELEMENT_H
