#include "tabsmodel.h"
#include "view.h"

TabsModel::TabsModel(View *view, QObject *parent) :
    QAbstractListModel(parent), mView(view)
{
}


int TabsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

int TabsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
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
    Q_UNUSED(role);
    mView->tabs()->at( index.row() )->setName( value.toString() );
    return true;
}

Qt::ItemFlags TabsModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
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

void TabsModel::addTab(const QString &name)
{
    QAbstractListModel::beginInsertRows(QModelIndex(), mView->tabs()->count(), mView->tabs()->count() );
    mView->tabs()->append(new Tab(name));
    QAbstractListModel::endInsertRows();
}

void TabsModel::removeTab(const QModelIndex &index)
{
    QAbstractListModel::beginRemoveRows(QModelIndex(), index.row(), index.row());
    mView->tabs()->removeAt(index.row());
    QAbstractListModel::endRemoveRows();
}
