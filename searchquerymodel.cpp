#include "searchquerymodel.h"

#include <QSet>
#include <QFileInfo>
#include <QXmlResultItems>
#include <QDomDocument>
#include <QXmlQuery>
#include <QDebug>

#include "messagehandler.h"

SearchQueryModel::SearchQueryModel( const QString & queryString, const QSet<QString>* textPaths, QObject * parent, const QList<Focus> & focus ) : QStandardItemModel( parent )
{
    mQuery = queryString;
    mFocus = focus;

    QStandardItem *parentItem = invisibleRootItem();

    QStringList paths( QList<QString>::fromSet(*textPaths) );
    paths.sort();

    QStringListIterator iter(paths);
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
    query.setMessageHandler(new MessageHandler("SearchQueryModel::query",this));
    query.bindVariable("path", QVariant(QUrl(filename).path(QUrl::FullyEncoded)));
    query.setQuery(mQuery);
    query.evaluateTo(&result);

    QList<QVariant> types;
    QList<QVariant> indices;
    for(int i=0; i<mFocus.count(); i++)
    {
        types << mFocus.at(i).type();
        indices << mFocus.at(i).index();
    }

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
            QString stringResult = item.toAtomicValue().toString();
            QString displayString;
            int lineNumber;
            if( stringResult.contains(",") )
            {
                QStringList bits = stringResult.split(",");
                if( bits.at(1).toInt() > 1 )
                    displayString = tr("Line %1 (%2)").arg(bits.at(0)).arg(bits.at(1));
                else
                    displayString = tr("Line %1").arg(bits.at(0));
                lineNumber = bits.at(0).toInt();
            }
            else
            {
                displayString = tr("Line %1").arg(stringResult);
                lineNumber = stringResult.toInt();
            }

            QStandardItem *resultItem = new QStandardItem( displayString );
            resultItem->setEditable(false);
            resultItem->setData( lineNumber , Qt::UserRole );
            resultItem->setData( types , Focus::TypeList );
            resultItem->setData( indices , Focus::IndexList );
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
