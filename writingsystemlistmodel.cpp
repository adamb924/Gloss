#include "writingsystemlistmodel.h"

WritingSystemListModel::WritingSystemListModel(QList<WritingSystem> *list, QObject *parent) :
    QAbstractListModel(parent), mList(list)
{
}

int WritingSystemListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

int WritingSystemListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mList->count();
}

QVariant WritingSystemListModel::data(const QModelIndex &index, int role) const
{
    if ( role == Qt::DisplayRole || role == Qt::EditRole )
    {
        return mList->at(index.row()).name();
    }
    return QVariant();
}

Qt::ItemFlags WritingSystemListModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

void WritingSystemListModel::setWritingSystem(const QModelIndex &index, const WritingSystem &ws)
{
    mList->replace(index.row(), ws);
    emit dataChanged(index, index);
}

void WritingSystemListModel::addWritingSystem(const WritingSystem &ws)
{
    QAbstractListModel::beginInsertRows(QModelIndex(), mList->count(), mList->count() );
    mList->append(ws);
    QAbstractListModel::endInsertRows();
}

void WritingSystemListModel::removeWritingSystem(const QModelIndex &index)
{
    QAbstractListModel::beginRemoveRows(QModelIndex(), index.row(), index.row());
    mList->removeAt(index.row());
    QAbstractListModel::endRemoveRows();
}
