#ifndef TAGMODEL_H
#define TAGMODEL_H

#include <QStringListModel>

class DatabaseAdapter;

class TagModel : public QStringListModel
{
    Q_OBJECT
public:
    TagModel(const DatabaseAdapter * dbAdapter, QObject *parent = 0);

    Qt::DropActions supportedDropActions() const;

    bool dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent );
    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    Qt::ItemFlags flags ( const QModelIndex & index ) const;

signals:

public slots:
    void setLexicalEntry( qlonglong lexicalEntryId = -1 );
    void refreshTags( );
private:
    const DatabaseAdapter * mDbAdapter;
    qlonglong mLexicalEntryId;
};

#endif // TAGMODEL_H
