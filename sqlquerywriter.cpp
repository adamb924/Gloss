#include "sqlquerywriter.h"

#include <QTextStream>
#include <QFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>

SqlQueryWriter::SqlQueryWriter(const QSqlQuery & query)
{
    mQuery = query;
}

bool SqlQueryWriter::serialize(const QString & filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly))
        return false;
    QTextStream stream(&file);

    QSqlRecord record = mQuery.record();
    for(int i=0; i<record.count(); i++)
    {
        stream << "\"" << record.fieldName( i ) << "\"";
        if( i != record.count() -1 )
            stream << ",";
    }
    stream << "\n";

    mQuery.first();
    mQuery.previous();

    while( mQuery.next() )
    {
        for(int i=0; i<record.count(); i++)
        {
            stream << "\"" << mQuery.value(i).toString().replace("\"","\"\"") << "\"";
            if( i != record.count() -1 )
                stream << ",";
        }
        stream << "\n";
    }

    return true;
}
