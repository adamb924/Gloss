/*!
  \class AnnotationTypeListModel
  \ingroup GUIModel
  \brief This is a model of a list of annotation types (i.e., as a property of a Project). It's currently used in ProjectOptionsDialog, with a simple QListView.
*/

#ifndef ANNOTATIONTYPELISTMODEL_H
#define ANNOTATIONTYPELISTMODEL_H

#include <QAbstractListModel>

class AnnotationType;

class AnnotationTypeListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    AnnotationTypeListModel(QList<AnnotationType> *list, QObject *parent = nullptr);

    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex & index) const;

    void setAnnotationType(const QModelIndex &index, const AnnotationType & type);
    void addAnnotationType(const AnnotationType & index);
    void removeAnnotationType(const QModelIndex &index);

signals:

public slots:

private:
    QList<AnnotationType> * mList;

};

#endif // ANNOTATIONTYPELISTMODEL_H
