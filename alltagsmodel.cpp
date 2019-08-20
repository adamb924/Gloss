#include "alltagsmodel.h"

#include "databaseadapter.h"

AllTagsModel::AllTagsModel(const DatabaseAdapter * dbAdapter, QWidget *parent) :
    QStandardItemModel(parent)
{
    mDbAdapter = dbAdapter;
    refreshTags( );
}


void AllTagsModel::refreshTags( )
{
    clear();

    QStringList tags = mDbAdapter->availableGrammaticalTags();
    for(int i=0; i<tags.count(); i++)
    {
        QStandardItem *item = new QStandardItem( tags.at(i) );
        item->setEditable(true);
        item->setCheckable(true);
        item->setUserTristate(true);
        item->setCheckState(Qt::PartiallyChecked);
        invisibleRootItem()->appendRow(item);
    }
}

bool AllTagsModel::removeRows ( int row, int count, const QModelIndex & parent )
{
    mDbAdapter->removeTag( index( row, 0 , parent ).data().toString() );
    return QStandardItemModel::removeRows( row, count, parent );
}

bool AllTagsModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
    QVariant myValue = value;
    Qt::CheckState state = itemFromIndex(index)->checkState();

    if( role == Qt::EditRole )
    {
        mDbAdapter->renameTag( index.data().toString() , value.toString() );
    }
    else if ( role == Qt::CheckStateRole )
    {
        if( state == Qt::Unchecked )
            myValue = Qt::PartiallyChecked;
        else if( value == Qt::PartiallyChecked )
            myValue = Qt::Checked;
        else if( value == Qt::Checked )
            myValue = Qt::Unchecked;
    }
    return QStandardItemModel::setData( index, myValue, role );
}

QStringList AllTagsModel::positiveTags() const
{
   QStringList tags;
   for(int i=0; i<rowCount(); i++)
       if( item(i)->checkState() == Qt::Checked )
           tags << item(i)->data(Qt::DisplayRole).toString();
   return tags;
}

QStringList AllTagsModel::negativeTags() const
{
    QStringList tags;
    for(int i=0; i<rowCount(); i++)
        if( item(i)->checkState() == Qt::Unchecked )
            tags << item(i)->data(Qt::DisplayRole).toString();
    return tags;
}
