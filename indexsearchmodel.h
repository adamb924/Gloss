#ifndef INDEXSEARCHMODEL_H
#define INDEXSEARCHMODEL_H

#include <QStandardItemModel>
#include <QSqlQuery>
#include <QList>

class Focus;

class IndexSearchModel : public QStandardItemModel
{
public:
    IndexSearchModel( QSqlQuery query , const QList<Focus> & focus = QList<Focus>() );

private:
    QSqlQuery mQuery;
};

#endif // INDEXSEARCHMODEL_H
