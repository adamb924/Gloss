#include "viewsmodel.h"
#include "project.h"

#include <QMimeData>
#include <QtDebug>

ViewsModel::ViewsModel(Project *project, QObject *parent) : mProject(project),
    QAbstractListModel(parent)
{
}

int ViewsModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int ViewsModel::rowCount(const QModelIndex &parent) const
{
    return mProject->views()->count();
}

QVariant ViewsModel::data(const QModelIndex &index, int role) const
{
    if ( role == Qt::DisplayRole || role == Qt::EditRole )
    {
        return mProject->views()->at( index.row() )->name();
    }
    return QVariant();
}

bool ViewsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    mProject->views()->at( index.row() )->setName( value.toString() );
}

Qt::ItemFlags ViewsModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled;
}

void ViewsModel::addView(const QString &name)
{
    QAbstractListModel::beginInsertRows(QModelIndex(), mProject->views()->count(), mProject->views()->count() );
    mProject->views()->append( new View(name) );
    QAbstractListModel::endInsertRows();
}

void ViewsModel::removeView(const QModelIndex &index)
{
    QAbstractListModel::beginRemoveRows(QModelIndex(), index.row(), index.row());
    mProject->views()->removeAt(index.row());
    QAbstractListModel::endRemoveRows();
}

void ViewsModel::moveUp(int index)
{
    if( index > 0 )
    {
        QAbstractItemModel::beginMoveRows(QModelIndex(), index, index, QModelIndex(), index-1);
        mProject->views()->swap( index, index-1 );
        QAbstractItemModel::endMoveRows();
    }
}

void ViewsModel::moveDown(int index)
{
    if( index+1 < mProject->views()->count() )
    {
        QAbstractItemModel::beginMoveRows(QModelIndex(), index+1, index+1, QModelIndex(), index);
        mProject->views()->swap( index, index+1 );
        QAbstractItemModel::endMoveRows();
    }
}
