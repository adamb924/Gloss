#include "tagmodel.h"

#include <QSqlDriver>

#include "databaseadapter.h"

TagModel::TagModel(const DatabaseAdapter * dbAdapter, QObject *parent) :
        QStringListModel(parent)
{
    mDbAdapter = dbAdapter;
    mLexicalEntryId = -2;
}

void TagModel::setLexicalEntry( qlonglong lexicalEntryId )
{
    if( mLexicalEntryId == lexicalEntryId )
        return;

    mLexicalEntryId = lexicalEntryId;

    if( mLexicalEntryId == -1 ) // treat this as a request for all possible tags
        setStringList( mDbAdapter->availableGrammaticalTags() );
    else
        setStringList( mDbAdapter->grammaticalTags( mLexicalEntryId ) );
}
