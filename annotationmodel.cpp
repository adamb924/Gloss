#include "annotationmodel.h"
#include "text.h"

#include <QtDebug>

AnnotationModel::AnnotationModel(Text * text , const QString & annotationLabel, QObject *parent) :
    QAbstractListModel(parent)
{
    mText = text;
    setAnnotationType(annotationLabel);
}

void AnnotationModel::setAnnotationType(const QString & label)
{
    mAnnotationLabel = label;
    for(int i=0; i<mText->phrases()->count(); i++)
    {
        for(int j=0; j<mText->phrases()->at(i)->glossItemCount(); j++)
        {
            if( mText->phrases()->at(i)->glossItemAt(j)->hasAnnotation(label) )
            {
                mGlossItems << mText->phrases()->at(i)->glossItemAt(j);
            }
        }
    }
}

int AnnotationModel::rowCount(const QModelIndex & parent ) const
{
    return mGlossItems.count();
}

int AnnotationModel::columnCount(const QModelIndex & parent ) const
{
    return 1;
}

QVariant AnnotationModel::data(const QModelIndex & index, int role ) const
{
    if ( role == Qt::DisplayRole )
    {
        return mGlossItems.at( index.row() )->getAnnotation( mAnnotationLabel ).text();
    }
    return QVariant();
}

QVariant AnnotationModel::headerData(int section, Qt::Orientation orientation, int role ) const
{
    if( orientation == Qt::Horizontal )
        return QVariant(section);
    else
        return QVariant(section);
}
