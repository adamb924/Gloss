#include "syntacticanalysis.h"

#include "syntacticanalysiselement.h"

#include <QtDebug>

SyntacticAnalysis::SyntacticAnalysis(const QString &name)
    : mName(name)
{
}

void SyntacticAnalysis::createConstituent(const QString &label, QList<SyntacticAnalysisElement*> elements)
{
    if( elements.isEmpty() ) return;
    if( allTerminals(elements) && noneHaveParents(elements) ) /// if all of the elements are terminal nodes, we just add that to the analysis
    {
        SyntacticAnalysisElement *tmp = new SyntacticAnalysisElement(label, elements );
        mElements << tmp;
    }
    else if( areSisters(elements) )
    {
        findParent(elements[0])->replaceWithConstituent(label, elements);
    }
    else /// at least some of the nodes are constituents
    {
        if( anyHaveParents(elements) )
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
            mElements.insert( minIndex, new SyntacticAnalysisElement( label , elements ) );
        }

    }
}

const QList<SyntacticAnalysisElement *> *SyntacticAnalysis::elements() const
{
    return &mElements;
}

QString SyntacticAnalysis::name() const
{
    return mName;
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
            if( mElements.at(j)->hasDescendant(elements.at(i)) )
            {
                return true;
            }
        }
    }
    return false;
}

bool SyntacticAnalysis::noneHaveParents(QList<SyntacticAnalysisElement *> elements) const
{
    for(int i=0; i<elements.count(); i++)
    {
        for(int j=0; j<mElements.count(); j++)
        {
            if( mElements.at(j)->hasDescendant(elements.at(i)) )
            {
                return false;
            }
        }
    }
    return true;
}

bool SyntacticAnalysis::areSisters(QList<SyntacticAnalysisElement *> elements)
{
    if( elements.count() < 2 )
    {
        return true;
    }
    const SyntacticAnalysisElement * firstParent = findParent( elements.first() );
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
