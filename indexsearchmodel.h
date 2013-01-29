#ifndef INDEXSEARCHMODEL_H
#define INDEXSEARCHMODEL_H

#include <QStandardItemModel>
#include <QSqlQuery>

class IndexSearchModel : public QStandardItemModel
{
public:
    IndexSearchModel( QSqlQuery query );

private:
    QSqlQuery mQuery;
};

#endif // INDEXSEARCHMODEL_H
