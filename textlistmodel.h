#ifndef TEXTLISTMODEL_H
#define TEXTLISTMODEL_H

#include <QAbstractTableModel>

class Project;

class TextListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TextListModel(Project * project, QObject *parent = nullptr);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex & index) const;

signals:

public slots:
    void dataChanged();

private:
    Project * mProject;

};

#endif // TEXTLISTMODEL_H
