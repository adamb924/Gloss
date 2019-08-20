#include "syntacticanalysiselement.h"

#include "allomorph.h"
#include "databaseadapter.h"

#include <QtDebug>

SyntacticAnalysisElement::SyntacticAnalysisElement(const Allomorph * allomorph, const DatabaseAdapter *dbAdapter)
    : mAllomorph(allomorph),
      mType(SyntacticAnalysisElement::Terminal),
      mParent(nullptr),
      mDbAdapter(dbAdapter)
{
}

SyntacticAnalysisElement::SyntacticAnalysisElement(const SyntacticType & type, const QList<SyntacticAnalysisElement *> &elements, const DatabaseAdapter * dbAdapter)
    : mAllomorph(nullptr),
      mSyntacticType(type),
      mType(SyntacticAnalysisElement::Consituent),
      mParent(nullptr),
      mDbAdapter(dbAdapter)
{
    for(int i=0; i<elements.count(); i++)
    {
        mElements << elements[i];
        elements[i]->setParent(this);
    }
}

SyntacticAnalysisElement::SyntacticAnalysisElement(const SyntacticType &type, SyntacticAnalysisElement * soleChild, const DatabaseAdapter * dbAdapter )
    : mAllomorph(nullptr),
      mSyntacticType(type),
      mType(SyntacticAnalysisElement::Consituent),
      mParent(nullptr),
      mDbAdapter(dbAdapter)
{
    mElements << soleChild;
    soleChild->setParent(this);
}

SyntacticAnalysisElement::~SyntacticAnalysisElement()
{
    for(int i=0; i<mElements.count(); i++)
    {
        mElements[i]->setParent(nullptr);
    }
    if( mParent != nullptr )
    {
        mParent->removeChild(this);
    }
}

QString SyntacticAnalysisElement::label() const
{
    return mSyntacticType.abbreviation();
}

const Allomorph *SyntacticAnalysisElement::allomorph() const
{
    return mAllomorph;
}

QList<SyntacticAnalysisElement *> *SyntacticAnalysisElement::children()
{
    return &mElements;
}

const QList<SyntacticAnalysisElement *> *SyntacticAnalysisElement::children() const
{
    return &mElements;
}

bool SyntacticAnalysisElement::hasChild() const
{
    return mElements.count() > 0;
}

bool SyntacticAnalysisElement::hasParent() const
{
    return mParent != nullptr;
}

SyntacticAnalysisElement *SyntacticAnalysisElement::parent()
{
    return mParent;
}

bool SyntacticAnalysisElement::isTerminal() const
{
    return mType == SyntacticAnalysisElement::Terminal;
}

bool SyntacticAnalysisElement::isConstituent() const
{
    return mType == SyntacticAnalysisElement::Consituent;
}

void SyntacticAnalysisElement::removeChild(SyntacticAnalysisElement *element)
{
    mElements.removeAll(element);
}

bool SyntacticAnalysisElement::hasChild( SyntacticAnalysisElement * element ) const
{
    return mElements.contains(element);
}

void SyntacticAnalysisElement::addChild(SyntacticAnalysisElement *element)
{
    element->setParent(this);
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
        qWarning() << "SyntacticAnalysisElement Begin Constituent" << mSyntacticType.abbreviation();
        for(int i=0; i<mElements.count(); i++)
        {
            mElements.at(i)->debug();
        }
        qWarning() << "SyntacticAnalysisElement End Constituent" << mSyntacticType.abbreviation();
    }
}

void SyntacticAnalysisElement::setParent(SyntacticAnalysisElement *parent)
{
    mParent = parent;
}

void SyntacticAnalysisElement::replaceWithConstituent(const SyntacticType &type, QList<SyntacticAnalysisElement *> &elements)
{
    int minIndex = mElements.indexOf( elements.first() );
    QListIterator<SyntacticAnalysisElement*> iter(elements);
    while(iter.hasNext())
    {
        SyntacticAnalysisElement* element = iter.next();
        minIndex = qMin( minIndex , mElements.indexOf( element ) );
        mElements.removeAll( element );
    }
    SyntacticAnalysisElement * topElement = new SyntacticAnalysisElement( type , elements, mDbAdapter );
    while( !topElement->type().automaticParent().isEmpty() )
    {
        topElement = new SyntacticAnalysisElement( mDbAdapter->syntacticType( topElement->type().automaticParent() ) , topElement, mDbAdapter );
    }
    mElements.insert( minIndex, topElement );
}

SyntacticType SyntacticAnalysisElement::type() const
{
    return mSyntacticType;
}


QDebug operator<<(QDebug dbg, const SyntacticAnalysisElement &key)
{
    if( key.isTerminal() )
        dbg.nospace() << "SyntacticAnalysisElement (terminal)" << * key.allomorph();
    else
        dbg.nospace() << "SyntacticAnalysisElement (constituent)" << * key.children();
    return dbg.maybeSpace();
}
