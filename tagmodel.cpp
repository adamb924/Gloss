#include "tagmodel.h"

#include <QSqlDriver>

#include "databaseadapter.h"

TagModel::TagModel(const DatabaseAdapter * dbAdapter, QObject *parent) :
        QStringListModel(parent)
{
    mDbAdapter = dbAdapter;
    mLexicalEntryId = -1;
}

void TagModel::setLexicalEntry( qlonglong lexicalEntryId )
{
    if( mLexicalEntryId == lexicalEntryId || lexicalEntryId == -1 )
        return;
    mLexicalEntryId = lexicalEntryId;
    refreshTags();
}

void TagModel::refreshTags( )
{
    setStringList( mDbAdapter->grammaticalTags( mLexicalEntryId ) );
}

Qt::DropActions TagModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

bool TagModel::dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent )
{
    bool retVal = QStringListModel::dropMimeData( data, action, row, column, parent );
    if( mLexicalEntryId != -1 )
        mDbAdapter->setTagsForLexicalEntry( mLexicalEntryId , stringList() );
    return retVal;
}

bool TagModel::removeRows ( int row, int count, const QModelIndex & parent )
{
    bool retVal = QStringListModel::removeRows( row, count, parent );
    if( mLexicalEntryId != -1 )
        mDbAdapter->setTagsForLexicalEntry( mLexicalEntryId , stringList() );
    return retVal;
}

Qt::ItemFlags TagModel::flags ( const QModelIndex & index ) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
}
