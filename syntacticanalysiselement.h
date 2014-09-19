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

    QString label() const;
    const Allomorph * allomorph() const;
    QList<SyntacticAnalysisElement *> *elements();
    const QList<SyntacticAnalysisElement *> *elements() const;
    bool isTerminal() const;
    bool isConstituent() const;
    bool hasDescendant( const SyntacticAnalysisElement * element ) const;
    bool removeDescendant(SyntacticAnalysisElement * element);
    bool hasChild(SyntacticAnalysisElement *element ) const;
    void addChild(SyntacticAnalysisElement *element);
    void debug() const;

    //! \brief Returns the address of the object if it has \a element as a child, or tries to find it in its descendants
    const SyntacticAnalysisElement * findParent(SyntacticAnalysisElement *element ) const;
    SyntacticAnalysisElement * findParent(SyntacticAnalysisElement *element );

    void replaceWithConstituent(const QString & label, QList<SyntacticAnalysisElement *> &elements);

private:
    const Allomorph *mAllomorph;
    SyntacticType mSyntacticType;
    SyntacticAnalysisElement::Type mType;
    QList<SyntacticAnalysisElement *> mElements;
};

QDebug operator<<(QDebug dbg, const SyntacticAnalysisElement &key);

#endif // SYNTACTICANALYSISELEMENT_H
