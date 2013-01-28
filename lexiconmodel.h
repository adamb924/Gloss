#ifndef LEXICONMODEL_H
#define LEXICONMODEL_H

#include <QAbstractItemModel>

#include <QSqlQuery>

class DatabaseAdapter;
class TreeItem;

class LexiconModel : public QAbstractItemModel
{
public:
    LexiconModel( const DatabaseAdapter * dbAdapter );
    ~LexiconModel();

private:
    const DatabaseAdapter * mDbAdapter;
    void populateModel();

    QModelIndex	index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex	parent ( const QModelIndex & index ) const;
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    int	columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QList<qlonglong> mIds;

    TreeItem *mRootItem;
};

#endif // LEXICONMODEL_H
