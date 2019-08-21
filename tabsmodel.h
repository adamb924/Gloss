/*!
  \class TabsModel
  \ingroup GUIModel
  \brief This is a model of a list of Tab objects (i.e., as child elements of a View). It's currently used in ViewConfigurationDialog, with a simple QListView.
*/

#ifndef TABSMODEL_H
#define TABSMODEL_H

#include <QAbstractListModel>

#include "tab.h"

class View;

class TabsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    TabsModel(View * view, QObject *parent = nullptr);

    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex & index) const;

    void moveUp(int index);
    void moveDown(int index);

    void addTab(const QString & name, Tab::TabType type);
    void removeTab(const QModelIndex & index);

private:
    View * mView;

signals:

public slots:

};

#endif // TABSMODEL_H
