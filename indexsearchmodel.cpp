#include "indexsearchmodel.h"

IndexSearchModel::IndexSearchModel( QSqlQuery query )
{
    mQuery = query;

    QStandardItem *parentItem = invisibleRootItem();

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
        filenameItem->appendRow(resultItem);

        previousTextName = textName;
    }
}
