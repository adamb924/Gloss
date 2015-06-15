/*!
  \class SqlQueryWriter
  \ingroup IO
  \brief This class writes the results of an executed QSqlQuery to a CSV-formatted file.
*/

#ifndef SQLQUERYWRITER_H
#define SQLQUERYWRITER_H

#include <QSqlQuery>
class QString;

class SqlQueryWriter
{
public:
    explicit SqlQueryWriter(const QSqlQuery & query);

    bool serialize(const QString & filePath);

private:
    QSqlQuery mQuery;
};

#endif // SQLQUERYWRITER_H
