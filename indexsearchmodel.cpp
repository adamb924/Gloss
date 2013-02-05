#include "indexsearchmodel.h"

#include "focus.h"

IndexSearchModel::IndexSearchModel( QSqlQuery query, const QList<Focus> & focus )
{
    mQuery = query;

    QStandardItem *parentItem = invisibleRootItem();

    QList<QVariant> types;
    QList<QVariant> indices;
    for(int i=0; i<focus.count(); i++)
    {
        types << focus.at(i).type();
        indices << focus.at(i).index();
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
        filenameItem->appendRow(resultItem);

        previousTextName = textName;
    }
}
