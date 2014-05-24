#ifndef TABSMODEL_H
#define TABSMODEL_H

#include <QAbstractListModel>

class View;

class TabsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit TabsModel(View * view, QObject *parent = 0);

    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex & index) const;

    void moveUp(int index);
    void moveDown(int index);

private:
    View * mView;

signals:

public slots:

};

#endif // TABSMODEL_H
