#include "phrasalglossesmodel.h"
#include "tab.h"

PhrasalGlossesModel::PhrasalGlossesModel(Tab *tab, QObject *parent) :
    mTab(tab), QAbstractListModel(parent)
{
}

void PhrasalGlossesModel::addPhrasalGloss(const WritingSystem &ws)
{
    QAbstractListModel::beginInsertRows(QModelIndex(), mTab->phrasalGlossLines()->count(), mTab->phrasalGlossLines()->count());
    mTab->addPhrasalGlossType( InterlinearItemType(InterlinearItemType::Gloss, ws) );
    QAbstractListModel::endInsertRows();
}

void PhrasalGlossesModel::removePhrasalGloss(int index)
{
    QAbstractListModel::beginRemoveRows(QModelIndex(), index, index);
    mTab->removePhrasalGloss(index);
    QAbstractListModel::endRemoveRows();
}

void PhrasalGlossesModel::editPhrasalGloss(const QModelIndex &index, const WritingSystem &ws)
{
    mTab->setPhrasalGloss(index.row(), InterlinearItemType(InterlinearItemType::Gloss, ws) );
    emit dataChanged( index, index );
}

int PhrasalGlossesModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int PhrasalGlossesModel::rowCount(const QModelIndex &parent) const
{
    return mTab->phrasalGlossLines()->count();
}

QVariant PhrasalGlossesModel::data(const QModelIndex &index, int role) const
{
    if ( role == Qt::DisplayRole )
    {
        return mTab->phrasalGlossLines()->at( index.row() ).writingSystem().name();
    }
    return QVariant();
}

Qt::ItemFlags PhrasalGlossesModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

void PhrasalGlossesModel::moveUp(int index)
{
    if( index > 0 )
    {
        QAbstractItemModel::beginMoveRows(QModelIndex(), index, index, QModelIndex(), index-1);
        mTab->phrasalGlossLines()->swap( index, index-1 );
        QAbstractItemModel::endMoveRows();
    }
}

void PhrasalGlossesModel::moveDown(int index)
{
    if( index+1 < mTab->phrasalGlossLines()->count() )
    {
        QAbstractItemModel::beginMoveRows(QModelIndex(), index+1, index+1, QModelIndex(), index);
        mTab->phrasalGlossLines()->swap( index, index+1 );
        QAbstractItemModel::endMoveRows();
    }
}
