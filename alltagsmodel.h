#ifndef ALLTAGSMODEL_H
#define ALLTAGSMODEL_H

#include <QStringListModel>

class DatabaseAdapter;

class AllTagsModel : public QStringListModel
{
    Q_OBJECT
public:
    AllTagsModel(const DatabaseAdapter * dbAdapter, QWidget *parent = 0);

    void refreshTags( );

    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

signals:

public slots:

private:
    const DatabaseAdapter * mDbAdapter;

};

#endif // ALLTAGSMODEL_H
