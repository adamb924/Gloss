#include "tab.h"

Tab::Tab(const QString &name, TabType type) : mName(name), mType(type)
{
}

Tab::~Tab()
{
    qDeleteAll(mInterlinearLines);
}

QString Tab::name() const
{
    return mName;
}

void Tab::setName(const QString &name)
{
    mName = name;
}

void Tab::addInterlinearLineType( const WritingSystem & ws, const InterlinearItemType & type )
{
    if( !mInterlinearLines.keys().contains(ws) )
    {
        mInterlinearLines[ws] = new InterlinearItemTypeList;
    }
    mInterlinearLines[ws]->append( type );
}

void Tab::removeInterlinearLineType(const WritingSystem &ws, int index)
{
    if( !mInterlinearLines.keys().contains(ws) )
    {
        mInterlinearLines[ws] = new InterlinearItemTypeList;
    }
    mInterlinearLines[ws]->removeAt(index);
}

void Tab::editInterlinearLineType(const WritingSystem &ws, int index, const InterlinearItemType &type)
{
    if( !mInterlinearLines.keys().contains(ws) )
    {
        mInterlinearLines[ws] = new InterlinearItemTypeList;
    }
    mInterlinearLines[ws]->replace(index, type);
}

void Tab::addPhrasalGlossType(const InterlinearItemType & type )
{
    mPhrasalGlossLines.append( type );
}

void Tab::removePhrasalGloss(int index)
{
    mPhrasalGlossLines.removeAt(index);
}

void Tab::setPhrasalGloss(int index, const InterlinearItemType &type)
{
    mPhrasalGlossLines.replace(index, type);
}

InterlinearItemTypeList Tab::interlinearLines(const WritingSystem &ws) const
{
    if( mInterlinearLines.contains(ws) )
    {
        return * mInterlinearLines.value(ws);
    }
    else
    {
        return InterlinearItemTypeList();
    }
}

QList<WritingSystem> Tab::interlinearLineKeys() const
{
    return mInterlinearLines.keys();
}

void Tab::interlinearLineSwap(const WritingSystem &ws, int i, int j)
{
    if( mInterlinearLines.contains(ws) )
    {
        mInterlinearLines[ws]->swap(i,j);
    }
}

const InterlinearItemTypeList* Tab::phrasalGlossLines() const
{
    return &mPhrasalGlossLines;
}

Tab::TabType Tab::type() const
{
    return mType;
}

QString Tab::getTypeString(Tab::TabType t)
{
    switch(t)
    {
    case Tab::InterlinearDisplay:
        return "interlinear-display";
    case Tab::SyntacticParsing:
        return "syntactic-parsing";
    case Tab::Null:
    default:
        return "null";
    }
}

Tab::TabType Tab::getType(QString t)
{
    if( t == "interlinear-display" )
        return Tab::InterlinearDisplay;
    else if ( t == "syntactic-parsing" )
        return Tab::SyntacticParsing;
    else
        return Tab::Null;
}

InterlinearItemTypeList* Tab::phrasalGlossLines()
{
    return &mPhrasalGlossLines;
}
