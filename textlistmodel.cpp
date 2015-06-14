#include "textlistmodel.h"

#include <QtDebug>

#include "project.h"

TextListModel::TextListModel(const Project *project, QObject *parent) :
    QAbstractListModel(parent),
    mProject(project)
{
}

int TextListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mProject->textCount();
}

QVariant TextListModel::data(const QModelIndex &index, int role) const
{
    if ( index.isValid() && ( role == Qt::DisplayRole || role == Qt::EditRole ) )
    {
        QStringList names = mProject->textNames();
        Q_ASSERT(index.row() < names.count());
        return names.at( index.row() );
    }
    return QVariant();
}

void TextListModel::dataChanged()
{
    emit beginResetModel();
    emit endResetModel();
}
