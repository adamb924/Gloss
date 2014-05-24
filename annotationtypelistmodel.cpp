#include "annotationtypelistmodel.h"

#include "annotationtype.h"

AnnotationTypeListModel::AnnotationTypeListModel(QList<AnnotationType> * list, QObject *parent) :
    mList(list), QAbstractListModel(parent)
{
}

int AnnotationTypeListModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int AnnotationTypeListModel::rowCount(const QModelIndex &parent) const
{
    return mList->count();
}

QVariant AnnotationTypeListModel::data(const QModelIndex &index, int role) const
{
    if ( role == Qt::DisplayRole || role == Qt::EditRole )
    {
        return tr("%1 (%2) (%3)").arg( mList->at(index.row()).label() ).arg(mList->at(index.row()).mark()).arg(mList->at(index.row()).writingSystem().name());
    }
    return QVariant();
}

Qt::ItemFlags AnnotationTypeListModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

void AnnotationTypeListModel::setAnnotationType(const QModelIndex &index, const AnnotationType &type)
{
    mList->replace(index.row(), type);
    emit dataChanged(index, index);
}

void AnnotationTypeListModel::addAnnotationType(const AnnotationType &index)
{
    QAbstractListModel::beginInsertRows(QModelIndex(), mList->count(), mList->count() );
    mList->append( index );
    QAbstractListModel::endInsertRows();
}

void AnnotationTypeListModel::removeAnnotationType(const QModelIndex &index)
{
    QAbstractListModel::beginRemoveRows(QModelIndex(), index.row(), index.row());
    mList->removeAt(index.row());
    QAbstractListModel::endRemoveRows();
}
