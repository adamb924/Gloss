#include "tab.h"

Tab::Tab(const QString &name) : mName(name)
{
}

QString Tab::name() const
{
    return mName;
}

void Tab::addInterlinearLineType( const WritingSystem & ws, const InterlinearItemType & type )
{
    mInterlinearLines[ws].append( type );
}

void Tab::addPhrasalGlossType(const InterlinearItemType & type )
{
    mPhrasalGlossLines.append( type );
}

QHash<WritingSystem,InterlinearItemTypeList> Tab::interlinearLines() const
{
    return mInterlinearLines;
}

InterlinearItemTypeList Tab::phrasalGlossLines() const
{
    return mPhrasalGlossLines;
}
