#include "itemsmodel.h"
#include "tab.h"

ItemsModel::ItemsModel(Tab *tab, WritingSystem &ws, QObject *parent) :
    mTab(tab), mWritingSystem(ws), QAbstractListModel(parent)
{
}

void ItemsModel::addItem(InterlinearItemType type, const WritingSystem & ws)
{
    if( mTab->interlinearLines().keys().contains(ws) )
    {
        QAbstractListModel::beginInsertRows(QModelIndex(), mTab->interlinearLines().value(ws)->count(), mTab->interlinearLines().value(ws)->count() );
    }
    else
    {
        QAbstractListModel::beginInsertRows(QModelIndex(), 0 , 0 );
    }
    mTab->addInterlinearLineType(ws, type);
    QAbstractListModel::endInsertRows();
}

void ItemsModel::removeItem(const WritingSystem & ws, const QModelIndex &index)
{
    QAbstractListModel::beginRemoveRows(QModelIndex(), index.row(), index.row());
    mTab->removeInterlinearLineType(ws, index.row() );
    QAbstractListModel::endRemoveRows();
}

void ItemsModel::editItem(const WritingSystem & ws, const QModelIndex &index, InterlinearItemType type)
{
    mTab->editInterlinearLineType(ws, index.row(), type);
    emit dataChanged(index, index);
}

int ItemsModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int ItemsModel::rowCount(const QModelIndex &parent) const
{
    if( mTab->interlinearLines().keys().contains( mWritingSystem ) )
    {
        return mTab->interlinearLines().value(mWritingSystem)->count();
    }
    else
    {
        return 0;
    }
}

QVariant ItemsModel::data(const QModelIndex &index, int role) const
{
    if ( role == Qt::DisplayRole )
    {
        return tr("%1 (%2)").arg( mTab->interlinearLines().value(mWritingSystem)->at( index.row() ).typeString() ).arg( mTab->interlinearLines().value(mWritingSystem)->at( index.row() ).writingSystem().name() );
    }
    return QVariant();
}

Qt::ItemFlags ItemsModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}


void ItemsModel::moveUp(const WritingSystem & ws, int index)
{
    if( index > 0 )
    {
        QAbstractItemModel::beginMoveRows(QModelIndex(), index, index, QModelIndex(), index-1);
        mTab->interlinearLines()[ws]->swap( index, index-1 );
        QAbstractItemModel::endMoveRows();
    }
}

void ItemsModel::moveDown(const WritingSystem & ws, int index)
{
    if( index+1 < mTab->interlinearLines().value(ws)->count() )
    {
        QAbstractItemModel::beginMoveRows(QModelIndex(), index+1, index+1, QModelIndex(), index);
        mTab->interlinearLines()[ws]->swap( index, index+1 );
        QAbstractItemModel::endMoveRows();
    }
}
