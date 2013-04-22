#ifndef ANNOTATIONMODEL_H
#define ANNOTATIONMODEL_H

#include <QAbstractListModel>
#include <QList>

#include "glossitem.h"

class Text;

class AnnotationModel : public QAbstractListModel
{
    Q_OBJECT

public:
    AnnotationModel(Text *text, const QString & annotationLabel, QObject *parent = 0);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

public slots:
    void setAnnotationType(const QString & label);

private:
    Text * mText;
    QString mAnnotationLabel;
    QList<GlossItem*> mGlossItems;
};

#endif // ANNOTATIONMODEL_H
