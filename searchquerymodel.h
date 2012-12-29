#ifndef SEARCHQUERYMODEL_H
#define SEARCHQUERYMODEL_H

#include <QStandardItemModel>

class QXmlResultItems;

class SearchQueryModel : public QStandardItemModel
{
public:
    SearchQueryModel( const QString & queryString, const QSet<QString>* textPaths, QObject * parent = 0 );

private:
    QString mQuery;

    bool query( QStandardItem *parentItem, const QString & filename );
};

#endif // SEARCHQUERYMODEL_H
