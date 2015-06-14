#ifndef TEXTLISTMODEL_H
#define TEXTLISTMODEL_H

#include <QAbstractListModel>

class Project;

class TextListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    TextListModel(const Project * project, QObject *parent = 0);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

signals:

public slots:
    void dataChanged();

private:
    const Project * mProject;

};

#endif // TEXTLISTMODEL_H
