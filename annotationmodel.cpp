#include "annotationmodel.h"
#include "text.h"
#include "phrase.h"
#include "paragraph.h"

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
    for(int i=0; i<mText->paragraphs()->count(); i++)
    {
        for(int j=0; j<mText->paragraphs()->at(i)->phrases()->count(); j++)
        {
            for(int k=0; k<mText->paragraphs()->at(i)->phrases()->at(j)->glossItemCount(); k++)
            {
                if( mText->paragraphs()->at(i)->phrases()->at(j)->glossItemAt(k)->hasAnnotation(mAnnotationLabel) )
                {
                    mGlossItems << mText->paragraphs()->at(i)->phrases()->at(j)->glossItemAt(k);
                }
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
    return 3;
}

QVariant AnnotationModel::data(const QModelIndex & index, int role ) const
{
    if ( role == Qt::DisplayRole || role == Qt::EditRole )
    {
        if( index.column() == 0 )
        {
            return mText->lineNumberForGlossItem( mGlossItems.at( index.row() ) ) + 1;
        }
        else if( index.column() == 1 )
        {
            return mGlossItems.at( index.row() )->getAnnotation( mAnnotationLabel ).header().text();
        }
        else if( index.column() == 2 )
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
            if( index.column() == 1 ) // editing the header
            {
                h.setText( value.toString() );
            }
            else if ( index.column() == 2 ) // editing the text
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
    if( index.column() < 1 )
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

GlossItem * AnnotationModel::glossItem( const QModelIndex & index ) const
{
    return mGlossItems.at( index.row() );
}
