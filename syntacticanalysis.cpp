#include "syntacticanalysis.h"

#include "syntacticanalysiselement.h"
#include "allomorph.h"
#include "text.h"
#include "phrase.h"
#include "glossitem.h"
#include "project.h"
#include "databaseadapter.h"
#include "paragraph.h"

#include <QtDebug>

SyntacticAnalysis::SyntacticAnalysis(const QString &name, const WritingSystem &ws, const Text * text, bool closedVocabulary)
    : mName(name),
      mWritingSystem(ws),
      mClosedVocabulary(closedVocabulary),
      mDbAdapter(text->project()->dbAdapter())
{
    for(int i=0; i<text->paragraphs()->count(); i++)
    {
        for(int j=0; j<text->paragraphs()->at(i)->phrases()->count(); j++)
        {
            const Phrase * phrase = text->paragraphs()->at(i)->phrases()->at(j);
            for(int k=0; k<phrase->glossItemCount(); k++)
            {
                const MorphologicalAnalysis * ma = phrase->glossItemAt(k)->morphologicalAnalysis(mWritingSystem);
                for(int l=0; l<ma->allomorphCount(); l++)
                {
                    addBaselineElement( new SyntacticAnalysisElement( ma->allomorph(l), mDbAdapter ) );
                }
            }
        }
    }
}

void SyntacticAnalysis::createConstituent(const SyntacticType &type, QList<SyntacticAnalysisElement*> elements)
{
    if( elements.isEmpty() ) return;

    bool bAreSisters = areSisters(elements);
    bool bAnyHaveParents = anyHaveParents(elements);
    bool bNoneHaveParents = !bAnyHaveParents;

    SyntacticAnalysisElement *newElement = 0;

    if( bNoneHaveParents ) /// if all of the elements are terminal nodes, we just add that to the analysis
    {
        /// create a new element with containing \a elements
        newElement = new SyntacticAnalysisElement(type, elements, mDbAdapter );
        mElements << newElement;

        /// remove each of \a elements from the baseline
        foreach( SyntacticAnalysisElement * e, elements )
        {
            mElements.removeAll(e);
        }
    }
    else if( bAreSisters )
    {
        elements[0]->parent()->replaceWithConstituent(type, elements);
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
            newElement = new SyntacticAnalysisElement( type , elements, mDbAdapter );
            mElements.insert( minIndex, newElement );
            mElementConcordance.insert( newElement->allomorph()->guid() , newElement );
            connect( newElement->allomorph(), SIGNAL(allomorphDestroyed(Allomorph*)), this, SLOT(removeAllomorphFromAnalysis(Allomorph*)) );
        }
    }
    if( newElement != 0 )
    {
        /// add an automatic parent, if one is specified
        if( !type.automaticParent().isEmpty() )
        {
            QList<SyntacticAnalysisElement*> thisList;
            thisList << newElement;
            createConstituent( mDbAdapter->syntacticType( type.automaticParent() ), thisList );
        }
    }
    emit modified();
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
        connect( element->allomorph(), SIGNAL(allomorphDestroyed(Allomorph*)), this, SLOT(removeAllomorphFromAnalysis(Allomorph*)) );
        emit modified();
    }
}

void SyntacticAnalysis::removeConstituentElement(SyntacticAnalysisElement *element)
{
    if( element->isTerminal() ) return; // you don't delete terminal elements
    if( element->hasParent() )
    {
        SyntacticAnalysisElement * parent = element->parent();
        foreach( SyntacticAnalysisElement * child, * element->children() )
        {
            parent->addChild( child );
            element->removeChild(child);
        }
        parent->removeChild(element);
    }
    else // no parent, add the children to the base element array
    {
        foreach( SyntacticAnalysisElement * child, * element->children() )
        {
            element->removeChild(child);
            mElements.append( child );
        }
    }
    mElements.removeAll(element);
    delete element;
    emit modified();
}

void SyntacticAnalysis::setName(const QString &name)
{
    mName = name;
    emit modified();
}

void SyntacticAnalysis::setWritingSystem(const WritingSystem &ws)
{
    mWritingSystem = ws;
    emit modified();
}

void SyntacticAnalysis::setClosedVocabulary(bool closed)
{
    emit modified();
    mClosedVocabulary = closed;
}

SyntacticAnalysisElement *SyntacticAnalysis::elementFromGuid(const QUuid & guid)
{
    return mElementConcordance.value(guid, 0);
}

void SyntacticAnalysis::refreshText(const Text *text)
{
    QList<QUuid> guids = mElementConcordance.keys();
    for(int i=0; i<text->paragraphs()->count(); i++)
    {
        for(int j=0; j<text->paragraphs()->at(i)->phrases()->count(); j++)
        {
            const Phrase * phrase = text->paragraphs()->at(i)->phrases()->at(j);
            for(int k=0; k<phrase->glossItemCount(); k++)
            {
                const MorphologicalAnalysis * ma = phrase->glossItemAt(k)->morphologicalAnalysis(mWritingSystem);
                for(int k=0; k<ma->allomorphCount(); k++)
                {
                    if( !guids.contains( ma->allomorph(k)->guid() ) )
                    {
                        addBaselineElement( new SyntacticAnalysisElement( ma->allomorph(k), mDbAdapter ) );
                    }
                }
            }
        }
    }
}

void SyntacticAnalysis::reparentElement(QList<SyntacticAnalysisElement*> elements, SyntacticAnalysisElement *newParent)
{
    foreach(SyntacticAnalysisElement * element, elements)
    {
        if( element->parent() != 0 )
        {
            element->parent()->removeChild( element );
        }
        newParent->addChild( element );
    }
    emit modified();
}

void SyntacticAnalysis::removeAllomorphFromAnalysis(Allomorph *allomorph)
{
    SyntacticAnalysisElement * element = mElementConcordance.value( allomorph->guid(), 0 );
    if( element != 0 )
    {
        delete element;
    }
}

QString SyntacticAnalysis::name() const
{
    return mName;
}

WritingSystem SyntacticAnalysis::writingSystem() const
{
    return mWritingSystem;
}

bool SyntacticAnalysis::hasNoBaselineElements() const
{
    return mElementConcordance.isEmpty();
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

bool SyntacticAnalysis::closedVocabulary() const
{
    return mClosedVocabulary;
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
        if( elements.at(i)->parent() != 0 )
        {
            return true;
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
    const SyntacticAnalysisElement * firstParent = elements.first()->parent();
    if( firstParent == 0 )
    {
        return false;
    }
    for(int i=1; i<elements.count(); i++)
    {
        if( elements.at(i)->parent() != firstParent )
        {
            return false;
        }
    }
    return true;
}
