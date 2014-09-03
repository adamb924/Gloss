#include "syntacticanalysis.h"

#include "syntacticanalysiselement.h"
#include "allomorph.h"
#include "text.h"
#include "phrase.h"
#include "glossitem.h"

#include <QtDebug>

SyntacticAnalysis::SyntacticAnalysis(const QString &name, const WritingSystem &ws, const Text * text)
    : mName(name),
      mWritingSystem(ws)
{
    for(int i=0; i<text->phrases()->count(); i++)
    {
        const Phrase * phrase = text->phrases()->at(i);
        for(int j=0; j<phrase->glossItemCount(); j++)
        {
            const MorphologicalAnalysis * ma = phrase->glossItemAt(j)->morphologicalAnalysis(mWritingSystem);
            for(int k=0; k<ma->allomorphCount(); k++)
            {
                addBaselineElement( new SyntacticAnalysisElement( ma->allomorph(k) ) );
            }
        }
    }
}

void SyntacticAnalysis::createConstituent(const QString &label, QList<SyntacticAnalysisElement*> elements)
{
    if( elements.isEmpty() ) return;

    bool bAllTerminals = allTerminals(elements);
    bool bAreSisters = areSisters(elements);
    bool bAnyHaveParents = anyHaveParents(elements);
    bool bNoneHaveParents = !bAnyHaveParents;

    qDebug() << bAllTerminals << bNoneHaveParents << bAreSisters << bAnyHaveParents;

    if( bNoneHaveParents ) /// if all of the elements are terminal nodes, we just add that to the analysis
    {
        /// create a new element with containing \a elements
        SyntacticAnalysisElement *tmp = new SyntacticAnalysisElement(label, elements );
        mElements << tmp;
        mElementConcordance.insert( tmp->allomorph()->guid() , tmp );
        /// remove each of \a elements from the baseline
        foreach( SyntacticAnalysisElement * e, elements )
        {
            mElements.removeAll(e);
        }
    }
    else if( bAreSisters )
    {
        qDebug() << "Elements are sisters" << findParent(elements[0]);
        findParent(elements[0])->replaceWithConstituent(label, elements);
    }
    else /// at least some of the nodes are constituents
    {
        if( bAnyHaveParents )
        {
            /// Some elements have parents; undefined behavior.
            return;
        }
        else
        {
            int minIndex = mElements.indexOf( elements.first() );
            QListIterator<SyntacticAnalysisElement*> iter(elements);
            while(iter.hasNext())
            {
                SyntacticAnalysisElement* element = iter.next();
                minIndex = qMin( minIndex , mElements.indexOf( element ) );
                mElements.removeAll( element );
            }
            SyntacticAnalysisElement * tmp = new SyntacticAnalysisElement( label , elements );
            mElements.insert( minIndex, tmp );
            mElementConcordance.insert( tmp->allomorph()->guid() , tmp );
        }

    }
}

const QList<SyntacticAnalysisElement *> *SyntacticAnalysis::elements() const
{
    return &mElements;
}

void SyntacticAnalysis::addBaselineElement(SyntacticAnalysisElement *element)
{
    mElements.append(element);
    if( element->isTerminal() )
    {
        mElementConcordance.insert( element->allomorph()->guid() , element );
    }
}

SyntacticAnalysisElement *SyntacticAnalysis::elementFromGuid(const QUuid & guid)
{
    return mElementConcordance.value(guid, 0);
}

QString SyntacticAnalysis::name() const
{
    return mName;
}

WritingSystem SyntacticAnalysis::writingSystem() const
{
    return mWritingSystem;
}

void SyntacticAnalysis::debug() const
{
    for(int i=0; i<mElements.count(); i++)
    {
        if( mElements.at(i)->isConstituent() )
        {
            mElements.at(i)->debug();
        }
    }
}

bool SyntacticAnalysis::isEmpty() const
{
    return mElements.count() == 0;
}

bool SyntacticAnalysis::allTerminals(QList<SyntacticAnalysisElement *> elements) const
{
    for(int i=0; i<elements.count(); i++)
    {
        if( !elements.at(i)->isTerminal() )
        {
            return false;
        }
    }
    return true;
}

bool SyntacticAnalysis::anyHaveParents(QList<SyntacticAnalysisElement *> elements) const
{
    for(int i=0; i<elements.count(); i++)
    {
        for(int j=0; j<mElements.count(); j++)
        {
            if( !mElements.at(j)->isTerminal() )
            {
                if( mElements.at(j)->hasDescendant(elements.at(i)) )
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool SyntacticAnalysis::areSisters(QList<SyntacticAnalysisElement *> elements)
{
    if( elements.count() < 2 )
    {
        return true;
    }
    const SyntacticAnalysisElement * firstParent = findParent( elements.first() );
    if( firstParent == 0 )
    {
        return false;
    }
    for(int i=1; i<elements.count(); i++)
    {
        if( ! firstParent->hasChild( elements.at(i) ) )
        {
            return false;
        }
    }
    return true;
}

SyntacticAnalysisElement *SyntacticAnalysis::findParent(SyntacticAnalysisElement *element)
{
    for(int i=0; i<mElements.count(); i++)
    {
        SyntacticAnalysisElement * putative = mElements.at(i)->findParent(element);
        if( putative != 0 )
        {
            return putative;
        }
    }
    return 0;
}
