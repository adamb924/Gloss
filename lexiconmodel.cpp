#include "lexiconmodel.h"

#include "databaseadapter.h"
#include "treeitem.h"

LexiconModel::LexiconModel(const DatabaseAdapter * dbAdapter)
{
    mDbAdapter = dbAdapter;

    mIds = mDbAdapter->lexicalEntryIds();

    mRootItem = new TreeItem( mDbAdapter, 0 );

    populateModel();
}

LexiconModel::~LexiconModel()
{
    delete mRootItem;
}

void LexiconModel::populateModel()
{
    for( int i=0; i< mIds.count(); i++ )
    {
        mRootItem->appendChild( new TreeItem( mDbAdapter, mRootItem ) );
    }
}

int LexiconModel::rowCount ( const QModelIndex & parent ) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = mRootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int LexiconModel::columnCount ( const QModelIndex & parent ) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return mRootItem->columnCount();
}

QVariant LexiconModel::data ( const QModelIndex & index, int role ) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags LexiconModel::flags ( const QModelIndex & index ) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex LexiconModel::index ( int row, int column, const QModelIndex & parent ) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = mRootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex LexiconModel::parent ( const QModelIndex & index ) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == mRootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

QVariant LexiconModel::headerData(int section, Qt::Orientation orientation, int role) const
 {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return tr("Section %1").arg(section);

    return QVariant();
}
