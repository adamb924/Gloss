/*!
  \class ItemsModel
  \ingroup GUIModel
  \brief This is a model of a list of interlinear items (i.e., as child elements of a Tab). It's currently used in ViewConfigurationDialog, with a simple QListView.
*/

#ifndef ITEMSMODEL_H
#define ITEMSMODEL_H

#include <QAbstractListModel>

#include "writingsystem.h"

class Tab;
class InterlinearItemType;

class ItemsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ItemsModel(Tab * tab, WritingSystem & ws, QObject *parent = nullptr);

    void addItem(InterlinearItemType type, const WritingSystem &ws );
    void removeItem(const WritingSystem &ws, const QModelIndex & index );
    void editItem( const WritingSystem & ws, const QModelIndex &index, InterlinearItemType type );

    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex & index) const;

    void moveUp(const WritingSystem & ws, int index);
    void moveDown(const WritingSystem & ws, int index);

signals:

public slots:

private:
    Tab * mTab;
    WritingSystem mWritingSystem;
};

#endif // ITEMSMODEL_H
