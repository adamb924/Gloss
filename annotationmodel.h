/*!
  \class AnnotationModel
  \ingroup GUIModel
  \brief This class is a model for displaying the annotations of a text. It is used in AnnotationForm.
*/

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

    GlossItem * glossItem( const QModelIndex & index ) const;

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex & index) const;

public slots:
    void setAnnotationType(const QString & label);
    void loadGlossItems();

private:

    Text * mText;
    QString mAnnotationLabel;
    QList<GlossItem*> mGlossItems;
};

#endif // ANNOTATIONMODEL_H
