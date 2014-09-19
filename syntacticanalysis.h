#ifndef SYNTACTICANALYSIS_H
#define SYNTACTICANALYSIS_H

#include <QObject>
#include <QString>
#include <QList>
#include <QHash>

#include "writingsystem.h"
#include <QUuid>

class SyntacticAnalysisElement;
class Text;
class Tab;
class Allomorph;
class QUuid;
class SyntacticType;
class DatabaseAdapter;

class SyntacticAnalysis : public QObject
{
    Q_OBJECT

public:
    SyntacticAnalysis(const QString & name, const WritingSystem & ws, const Text *text, bool closedVocabulary);

    void createConstituent(const SyntacticType &type, QList<SyntacticAnalysisElement*> elements);

    const QList<SyntacticAnalysisElement*>* elements() const;

    void addBaselineElement(SyntacticAnalysisElement * element);
    void removeElement(SyntacticAnalysisElement * element);

    void setName(const QString & name);
    void setWritingSystem(const WritingSystem & ws);
    void setClosedVocabulary(bool closed);

    SyntacticAnalysisElement *elementFromGuid(const QUuid & guid);

    QString name() const;

    WritingSystem writingSystem() const;

    void debug() const;

    bool isEmpty() const;

    bool closedVocabulary() const;

signals:
    void modified();

public slots:
    void reparentElement(QList<SyntacticAnalysisElement *> elements, SyntacticAnalysisElement * newParent);

private:

    //! \brief Returns true if all of the elements are terminal elements, otherwise returns false
    bool allTerminals(QList<SyntacticAnalysisElement *> elements) const;

    //! \brief Returns true if any of the elements are already parsed into constituents, otherwise returns false
    bool anyHaveParents(QList<SyntacticAnalysisElement *> elements) const;

    //! \brief Returns true if the elements are all sisters, otherwise returns false. Returns true if there are fewer than two elements
    bool areSisters(QList<SyntacticAnalysisElement *> elements);

    //! \brief Returns the address of the object if it has \a element as a child, or tries to find it in its descendants. If no parent is found, returns 0.
    SyntacticAnalysisElement *findParent(SyntacticAnalysisElement *element );

private:
    QString mName;
    WritingSystem mWritingSystem;
    bool mClosedVocabulary;
    QList<SyntacticAnalysisElement*> mElements;
    QHash<QUuid, SyntacticAnalysisElement*> mElementConcordance;
    const DatabaseAdapter * mDbAdapter;
};

#endif // SYNTACTICANALYSIS_H
