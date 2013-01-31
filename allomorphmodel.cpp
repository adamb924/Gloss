#include "allomorphmodel.h"

#include "databaseadapter.h"

AllomorphModel::AllomorphModel(const DatabaseAdapter * dbAdapter, QObject *parent) :
    QSqlQueryModel(parent)
{
    mDbAdapter = dbAdapter;
    mLexicalEntryId = -1;
}

void AllomorphModel::setLexicalEntry( qlonglong lexicalEntryId )
{
    if( mLexicalEntryId == lexicalEntryId )
        return;

    mLexicalEntryId = lexicalEntryId;

    QSqlQuery q(QSqlDatabase::database( mDbAdapter->dbFilename() ));
    q.prepare( "select Form,Name from Allomorph,WritingSystems where LexicalEntryId=:LexicalEntryId and Allomorph.WritingSystem=WritingSystems._id;" );
    q.bindValue(":LexicalEntryId", lexicalEntryId);
    q.exec();
    setQuery(q);
}
