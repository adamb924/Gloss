#include "searchquerymodel.h"

#include <QSet>
#include <QFileInfo>
#include <QXmlResultItems>
#include <QDomDocument>
#include <QXmlQuery>
#include <QDebug>

#include "messagehandler.h"

SearchQueryModel::SearchQueryModel( const QString & queryString, const QSet<QString>* textPaths, QObject * parent ) : QStandardItemModel( parent )
{
    mQuery = queryString;

    QStandardItem *parentItem = invisibleRootItem();
    QSetIterator<QString> iter(*textPaths);
    while(iter.hasNext())
    {
        QString filename = iter.next();
        if( !query( parentItem, filename ) )
            break;
    }
    if( !parentItem->hasChildren() )
    {
        QStandardItem *filenameItem = new QStandardItem( tr("No results") );
        parentItem->appendRow(filenameItem);
    }
}

bool SearchQueryModel::query( QStandardItem *parentItem, const QString & filename )
{
    QXmlResultItems result;
    QXmlQuery query(QXmlQuery::XQuery10);
    if(!query.setFocus(QUrl(filename)))
        return false;
    query.setMessageHandler(new MessageHandler(this));
    query.setQuery(mQuery);
    query.evaluateTo(&result);
    QXmlItem item(result.next());
    if( !item.isNull() )
    {
        QFileInfo info(filename);
        QStandardItem *filenameItem = new QStandardItem( info.baseName() );
        filenameItem->setEditable(false);
        filenameItem->setData( info.baseName(), Qt::UserRole );
        parentItem->appendRow(filenameItem);

        while (!item.isNull())
        {
            QStandardItem *resultItem = new QStandardItem( tr("Line %1").arg(item.toAtomicValue().toString()) );
            resultItem->setEditable(false);
            resultItem->setData( item.toAtomicValue().toInt(), Qt::UserRole );
            filenameItem->appendRow(resultItem);
            item = result.next();
        }
    }
    // strangely, we check only here because otherwise we don't get the error messages
    if( query.isValid() )
        return true;
    else
        return false;
}
