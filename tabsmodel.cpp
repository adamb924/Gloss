#include "tabsmodel.h"
#include "view.h"

TabsModel::TabsModel(View *view, QObject *parent) :
    mView(view), QAbstractListModel(parent)
{
}


int TabsModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int TabsModel::rowCount(const QModelIndex &parent) const
{
    return mView->tabs()->count();
}

QVariant TabsModel::data(const QModelIndex &index, int role) const
{
    if ( role == Qt::DisplayRole || role == Qt::EditRole )
    {
        return mView->tabs()->at(index.row())->name();
    }
    return QVariant();
}

bool TabsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    mView->tabs()->at( index.row() )->setName( value.toString() );
}

Qt::ItemFlags TabsModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

void TabsModel::moveUp(int index)
{
    if( index > 0 )
    {
        QAbstractItemModel::beginMoveRows(QModelIndex(), index, index, QModelIndex(), index-1);
        mView->tabs()->swap( index, index-1 );
        QAbstractItemModel::endMoveRows();
    }
}

void TabsModel::moveDown(int index)
{
    if( index+1 < mView->tabs()->count() )
    {
        QAbstractItemModel::beginMoveRows(QModelIndex(), index+1, index+1, QModelIndex(), index);
        mView->tabs()->swap( index, index+1 );
        QAbstractItemModel::endMoveRows();
    }
}
