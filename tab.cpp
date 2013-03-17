#include "tab.h"

Tab::Tab(const QString &name)
{
    mName = name;
}

QString Tab::name() const
{
    return mName;
}

void Tab::addInterlinearLineType( const InterlinearItemType & type )
{
    mInterlinearLines.append( type );
}

void Tab::addPhrasalGlossType( const InterlinearItemType & type )
{
    mPhrasalGlossLines.append( type );
}

QList<InterlinearItemType> Tab::interlinearLines() const
{
    return mInterlinearLines;
}

QList<InterlinearItemType> Tab::phrasalGlossLines() const
{
    return mPhrasalGlossLines;
}
