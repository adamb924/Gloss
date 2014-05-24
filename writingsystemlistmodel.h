#ifndef WRITINGSYSTEMLISTMODEL_H
#define WRITINGSYSTEMLISTMODEL_H

#include <QAbstractListModel>
#include <QList>

#include "writingsystem.h"

class WritingSystemListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    WritingSystemListModel( QList<WritingSystem> * list, QObject *parent = 0);

    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex & index) const;

    void setWritingSystem(const QModelIndex &index, const WritingSystem & ws);
    void addWritingSystem(const WritingSystem & ws);
    void removeWritingSystem(const QModelIndex &index);

signals:

public slots:

private:
    QList<WritingSystem> * mList;

};

#endif // WRITINGSYSTEMLISTMODEL_H
