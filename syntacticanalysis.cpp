#include "syntacticanalysis.h"

#include "syntacticanalysiselement.h"

SyntacticAnalysis::SyntacticAnalysis(const QString &name)
    : mName(name)
{
}

void SyntacticAnalysis::createConstituent(const QString &label, QList<SyntacticAnalysisElement*> elements)
{
    if( elements.isEmpty() ) return;
    if( allTerminals(elements) )
    {
        SyntacticAnalysisElement *tmp = new SyntacticAnalysisElement(label, elements );
        mElements << tmp;
    }
    else
    {
        /// @todo some check here
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
