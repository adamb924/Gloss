#include "xquerymodel.h"

#include <QSet>
#include <QFileInfo>
#include <QXmlResultItems>
#include <QDomDocument>
#include <QXmlQuery>
#include <QDebug>

#include "messagehandler.h"

XQueryModel::XQueryModel( const QString & queryString, const QSet<QString>* textPaths, QObject * parent, const QList<Focus> & focus ) :
    QStandardItemModel( parent ), mQuery(queryString), mFocus(focus)
{
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

QString XQueryModel::resultSummary() const
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

bool XQueryModel::query( QStandardItem *parentItem, const QString & filename )
{
    QXmlResultItems result;
    QXmlQuery query(QXmlQuery::XQuery10);
    query.setMessageHandler(new MessageHandler("XQueryModel::query",this));
    query.bindVariable("path", QVariant(QUrl::fromLocalFile(filename).path(QUrl::FullyEncoded)));
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
            int count;
            int lineNumber;
            if( stringResult.contains(",") )
            {
                QStringList bits = stringResult.split(",");
                lineNumber = bits.at(0).toInt();
                count = bits.at(1).toInt();
                if( count > 1 )
                    displayString = tr("Line %1 (%2)").arg(lineNumber).arg(count);
                else
                    displayString = tr("Line %1").arg(lineNumber);
            }
            else
            {
                displayString = tr("Line %1").arg(stringResult);
                lineNumber = stringResult.toInt();
                count = 1;
            }

            QStandardItem *resultItem = new QStandardItem( displayString );
            resultItem->setEditable(false);
            resultItem->setData( lineNumber , Qt::UserRole );
            resultItem->setData( types , Focus::TypeList );
            resultItem->setData( indices , Focus::IndexList );
            resultItem->setData( count , Qt::UserRole + 3 );
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
