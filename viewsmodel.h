#ifndef VIEWSMODEL_H
#define VIEWSMODEL_H

#include <QAbstractListModel>

class Project;

class ViewsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ViewsModel(Project * project, QObject *parent = 0);

    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex & index) const;

    void addView(const QString & name);
    void removeView(const QModelIndex & index);

    void moveUp(int index);
    void moveDown(int index);

private:
    Project * mProject;

signals:

public slots:

};

#endif // VIEWSMODEL_H
