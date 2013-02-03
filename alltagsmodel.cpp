#include "alltagsmodel.h"

#include "databaseadapter.h"

AllTagsModel::AllTagsModel(const DatabaseAdapter * dbAdapter, QWidget *parent) :
    QStringListModel(parent)
{
    mDbAdapter = dbAdapter;
    refreshTags( );
}


void AllTagsModel::refreshTags( )
{
    setStringList( mDbAdapter->availableGrammaticalTags() );
}

bool AllTagsModel::removeRows ( int row, int count, const QModelIndex & parent )
{
    mDbAdapter->removeTag( index( row, 0 , parent ).data().toString() );
    bool retVal = QStringListModel::removeRows( row, count, parent );
    return retVal;
}

Qt::ItemFlags AllTagsModel::flags ( const QModelIndex & index ) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEditable;
}

bool AllTagsModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
    mDbAdapter->renameTag( index.data().toString() , value.toString() );
    return QStringListModel::setData( index, value, role );
}
