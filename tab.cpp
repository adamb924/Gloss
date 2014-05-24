#include "tab.h"

Tab::Tab(const QString &name) : mName(name)
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

QHash<WritingSystem,InterlinearItemTypeList*> Tab::interlinearLines() const
{
    return mInterlinearLines;
}

QHash<WritingSystem,InterlinearItemTypeList*> Tab::interlinearLines()
{
    return mInterlinearLines;
}

const InterlinearItemTypeList* Tab::phrasalGlossLines() const
{
    return &mPhrasalGlossLines;
}

InterlinearItemTypeList* Tab::phrasalGlossLines()
{
    return &mPhrasalGlossLines;
}
