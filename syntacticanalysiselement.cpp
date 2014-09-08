#include "syntacticanalysiselement.h"

#include "allomorph.h"

#include <QtDebug>

SyntacticAnalysisElement::SyntacticAnalysisElement(const Allomorph * allomorph)
    : mAllomorph(allomorph),
      mType(SyntacticAnalysisElement::Terminal)
{
}

SyntacticAnalysisElement::SyntacticAnalysisElement(const QString & label, const QList<SyntacticAnalysisElement *> &elements)
    : mAllomorph(0),
      mLabel(label),
      mType(SyntacticAnalysisElement::Consituent)
{
    for(int i=0; i<elements.count(); i++)
    {
        mElements << elements[i];
    }
}

QString SyntacticAnalysisElement::label() const
{
    return mLabel;
}

const Allomorph *SyntacticAnalysisElement::allomorph() const
{
    return mAllomorph;
}

QList<SyntacticAnalysisElement *> *SyntacticAnalysisElement::elements()
{
    return &mElements;
}

const QList<SyntacticAnalysisElement *> *SyntacticAnalysisElement::elements() const
{
    return &mElements;
}

bool SyntacticAnalysisElement::isTerminal() const
{
    return mType == SyntacticAnalysisElement::Terminal;
}

bool SyntacticAnalysisElement::isConstituent() const
{
    return mType == SyntacticAnalysisElement::Consituent;
}

bool SyntacticAnalysisElement::hasDescendant(const SyntacticAnalysisElement *element) const
{
    for(int i=0; i<mElements.count(); i++)
    {
        if( mElements.at(i) == element || mElements.at(i)->hasDescendant(element) )
        {
            return true;
        }
    }
    return false;
}

bool SyntacticAnalysisElement::removeDescendant(SyntacticAnalysisElement *element)
{
    if( mElements.contains( element ) )
    {
        mElements.removeAll(element);
        return true;
    }
    else
    {
        for(int i=0; i<mElements.count(); i++)
        {
            if( mElements.at(i)->removeDescendant(element) )
            {
                return true;
            }
        }
    }
    return false;
}

bool SyntacticAnalysisElement::hasChild( SyntacticAnalysisElement * element ) const
{
    return mElements.contains(element);
}

void SyntacticAnalysisElement::addChild(SyntacticAnalysisElement *element)
{
    mElements.append(element);
}

void SyntacticAnalysisElement::debug() const
{
    if( mType == SyntacticAnalysisElement::Terminal )
    {
        qWarning() << "SyntacticAnalysisElement(" << this << " ) Terminal Allomorph(" << mAllomorph << ")";
    }
    else /// constituent
    {
        qWarning() << "SyntacticAnalysisElement Begin Constituent" << mLabel;
        for(int i=0; i<mElements.count(); i++)
        {
            mElements.at(i)->debug();
        }
        qWarning() << "SyntacticAnalysisElement End Constituent" << mLabel;
    }
}

SyntacticAnalysisElement *SyntacticAnalysisElement::findParent(SyntacticAnalysisElement *element)
{
    if( mElements.contains(element) )
    {
        return this;
    }
    else
    {
        for(int i=0; i<mElements.count(); i++)
        {
            SyntacticAnalysisElement * putative = mElements.at(i)->findParent(element);
            if( putative != 0 )
            {
                return putative;
            }
        }
    }
    return 0;
}

const SyntacticAnalysisElement *SyntacticAnalysisElement::findParent(SyntacticAnalysisElement *element) const
{
    if( mElements.contains(element) )
    {
        return this;
    }
    else
    {
        for(int i=0; i<mElements.count(); i++)
        {
            const SyntacticAnalysisElement * putative = mElements.at(i)->findParent(element);
            if( putative != 0 )
            {
                return putative;
            }
        }
    }
    return 0;
}

void SyntacticAnalysisElement::replaceWithConstituent(const QString &label, QList<SyntacticAnalysisElement *> &elements)
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


QDebug operator<<(QDebug dbg, const SyntacticAnalysisElement &key)
{
    if( key.isTerminal() )
        dbg.nospace() << "SyntacticAnalysisElement (terminal)" << * key.allomorph();
    else
        dbg.nospace() << "SyntacticAnalysisElement (constituent)" << * key.elements();
    return dbg.maybeSpace();
}
