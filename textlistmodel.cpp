#include "textlistmodel.h"

#include <QtDebug>

#include "project.h"

TextListModel::TextListModel(Project *project, QObject *parent) :
    QAbstractTableModel(parent),
    mProject(project)
{
}

int TextListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mProject->textCount();
}

int TextListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant TextListModel::data(const QModelIndex &index, int role) const
{
    if ( index.isValid() && ( role == Qt::DisplayRole || role == Qt::EditRole ) )
    {

        QStringList names = mProject->textNames();
        Q_ASSERT(index.row() < names.count());
        switch( index.column() )
        {
        case 0:
            return names.at( index.row() );
        case 1:
            return mProject->text( names.at( index.row() ) )->comment();
        case 2:
            return mProject->text( names.at( index.row() ) )->exportFilename();
        }

    }
    return QVariant();
}

bool TextListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);
    if( !index.isValid() || role != Qt::EditRole )
        return false;

    QStringList names = mProject->textNames();
    Q_ASSERT(index.row() < names.count());
    switch( index.column() )
    {
    case 0:
        mProject->renameText( names.at( index.row() ) , value.toString() );
        break;
    case 1:
        mProject->text( names.at( index.row() ) )->setComment( value.toString() );
        break;
    case 2:
        mProject->text( names.at( index.row() ) )->setExportFilename( value.toString() );
        break;
    }
    emit dataChanged();
    return true;
}

Qt::ItemFlags TextListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsSelectable	| Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

void TextListModel::dataChanged()
{
    emit beginResetModel();
    emit endResetModel();
}
