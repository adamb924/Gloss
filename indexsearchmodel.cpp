#include "indexsearchmodel.h"

#include "focus.h"

#include <QtDebug>

IndexSearchModel::IndexSearchModel( QSqlQuery query, const QList<Focus> & focus )
{
    mQuery = query;

    QStandardItem *parentItem = invisibleRootItem();

    QList<QVariant> types;
    QList<QVariant> indices;
    for(int i=0; i<focus.count(); i++)
    {
        types << focus.at(i).type();

        if( focus.at(i).type() != Focus::WholeStringSearch && focus.at(i).type() != Focus::SubStringSearch )
        {
            indices << focus.at(i).index();
        }
        else
        {
            indices << focus.at(i).searchString();
        }
    }

    if( query.size() == 0 )
    {
        QStandardItem *filenameItem = new QStandardItem( tr("No results") );
        parentItem->appendRow(filenameItem);
        return;
    }

    QString previousTextName;
    QStandardItem *filenameItem = 0;
    while( query.next() )
    {
        QString textName = query.value(0).toString();
        int lineNumber = query.value(1).toLongLong();
        int count = query.value(2).toInt();

        if( textName != previousTextName )
        {
            filenameItem = new QStandardItem( textName );
            filenameItem->setEditable(false);
            filenameItem->setData( textName , Qt::UserRole );
            parentItem->appendRow(filenameItem);
        }

        QString displayString;
        if( count > 1 )
            displayString = tr("Line %1 (%2)").arg( lineNumber ).arg( count );
        else
            displayString = tr("Line %1").arg( lineNumber );

        QStandardItem *resultItem = new QStandardItem( displayString );
        resultItem->setEditable(false);
        resultItem->setData( lineNumber , Qt::UserRole );
        resultItem->setData( types , Focus::TypeList );
        resultItem->setData( indices , Focus::IndexList );
        resultItem->setData( count , Qt::UserRole + 3 );
        filenameItem->appendRow(resultItem);

        previousTextName = textName;
    }
}

QString IndexSearchModel::resultSummary() const
{
    int textCount = rowCount();
    int lineCount = 0;
    int instanceCount = 0;

    for(int i=0; i<textCount; i++)
    {
        QModelIndex textIndex = index(i,0);
        for( int j=0; j<rowCount(textIndex); j++)
        {
            lineCount++;

            QModelIndex lineIndex = index(j,0,textIndex);
            instanceCount += itemFromIndex(lineIndex)->data(Qt::UserRole + 3 ).toInt();
        }
    }

    if( instanceCount == 0 )
    {
        return tr("");
    }
    else
    {
        return tr("%1 results on %2 lines of %3 texts").arg(instanceCount).arg(lineCount).arg(textCount);
    }
}
