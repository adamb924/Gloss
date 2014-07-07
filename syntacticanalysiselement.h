#ifndef SYNTACTICANALYSISELEMENT_H
#define SYNTACTICANALYSISELEMENT_H

class Allomorph;
class QString;

#include <QList>
#include <QString>

class SyntacticAnalysisElement
{
public:
    enum Type { Consituent , Terminal };

    SyntacticAnalysisElement(const Allomorph * allomorph);
    SyntacticAnalysisElement(const QString & label, QList<SyntacticAnalysisElement *> &elements);

    QString label() const;
    const Allomorph * allomorph() const;
    QList<SyntacticAnalysisElement *> *elements();
    const QList<SyntacticAnalysisElement *> *elements() const;
    bool isTerminal() const;

private:
    const Allomorph *mAllomorph;
    QString mLabel;
    SyntacticAnalysisElement::Type mType;
    QList<SyntacticAnalysisElement *> mElements;
};

#endif // SYNTACTICANALYSISELEMENT_H
