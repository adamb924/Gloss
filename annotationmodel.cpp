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
    loadGlossItems();
}

void AnnotationModel::loadGlossItems()
{
    beginResetModel();
    mGlossItems.clear();
    for(int i=0; i<mText->phrases()->count(); i++)
    {
        for(int j=0; j<mText->phrases()->at(i)->glossItemCount(); j++)
        {
            if( mText->phrases()->at(i)->glossItemAt(j)->hasAnnotation(mAnnotationLabel) )
            {
                mGlossItems << mText->phrases()->at(i)->glossItemAt(j);
            }
        }
    }
    endResetModel();
}

int AnnotationModel::rowCount(const QModelIndex & parent ) const
{
    Q_UNUSED(parent);
    return mGlossItems.count();
}

int AnnotationModel::columnCount(const QModelIndex & parent ) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant AnnotationModel::data(const QModelIndex & index, int role ) const
{
    if ( role == Qt::DisplayRole || role == Qt::EditRole )
    {
        if( index.column() == 0 )
        {
            return mGlossItems.at( index.row() )->getAnnotation( mAnnotationLabel ).header().text();
        }
        else if( index.column() == 1 )
        {
            return mGlossItems.at( index.row() )->getAnnotation( mAnnotationLabel ).text().text();
        }
    }
    return QVariant();
}

QVariant AnnotationModel::headerData(int section, Qt::Orientation orientation, int role ) const
{
    Q_UNUSED(role);
    if( orientation == Qt::Horizontal )
        return QVariant(section);
    else
        return QVariant(section);
}

bool AnnotationModel::setData(const QModelIndex & index, const QVariant & value, int role )
{
    if( role == Qt::EditRole )
    {
        if( index.row() < mGlossItems.count() )
        {
            Annotation a = mGlossItems.at( index.row() )->getAnnotation(mAnnotationLabel);
            TextBit h = a.header();
            TextBit t = a.text();
            if( index.column() == 0 ) // editting the header
            {
                h.setText( value.toString() );
            }
            else if ( index.column() == 1 ) // editting the text
            {
                t.setText( value.toString() );
            }
            mGlossItems.at( index.row() )->setAnnotation( mAnnotationLabel, Annotation(h,t) );
        }
    }
    return false;
}

Qt::ItemFlags AnnotationModel::flags(const QModelIndex & index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

GlossItem * AnnotationModel::glossItem( const QModelIndex & index ) const
{
    return mGlossItems.at( index.row() );
}
