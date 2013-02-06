#include "morphologicalanalysismodel.h"

#include "databaseadapter.h"

MorphologicalAnalysisModel::MorphologicalAnalysisModel(const DatabaseAdapter * dbAdapter, QObject *parent) :
        QSqlQueryModel(parent)
{
    mDbAdapter = dbAdapter;

    mLexicalEntryId = -1;

    // TODO HACK
    mTextFormWs = mDbAdapter->writingSystem(5);
    mGlossWs = mDbAdapter->writingSystem(3);

}

void MorphologicalAnalysisModel::setLexicalEntry( qlonglong lexicalEntryId )
{
    if( mLexicalEntryId == lexicalEntryId )
        return;

    mLexicalEntryId = lexicalEntryId;

    QSqlQuery q(QSqlDatabase::database( mDbAdapter->dbFilename() ));

    q.prepare("select Focus.TextFormId as ID, Focus, TextForm, Gloss from (select TextFormId,group_concat( Transcription , ' ' ) as TextForm, group_concat( Gloss , '-' ) as Gloss from (select TextFormId,AllomorphId,Allomorph.Form as Transcription,LexicalEntryGloss.Form as Gloss from MorphologicalAnalysisMembers,Allomorph,LexicalEntryGloss where TextFormId in ( select TextFormId from MorphologicalAnalysisMembers where AllomorphId in (select _id from Allomorph where  LexicalEntryId=? and WritingSystem=?) ) and AllomorphId = Allomorph._id and Allomorph.LexicalEntryId = LexicalEntryGloss.LexicalEntryId and LexicalEntryGloss.WritingSystem=? order by TextFormId, AllomorphOrder) group by TextFormId ) as Concatenation left join  ( select TextFormId, Form as Focus from Allomorph,MorphologicalAnalysisMembers on Allomorph._id=MorphologicalAnalysisMembers.AllomorphId and LexicalEntryId=? ) as Focus on Focus.TextFormId = Concatenation.TextFormId;");
    q.addBindValue(mLexicalEntryId);
    q.addBindValue(mTextFormWs.id());
    q.addBindValue(mGlossWs.id());
    q.addBindValue(mLexicalEntryId);

    if( !q.exec() )
        qWarning() << q.lastError().text() << q.executedQuery();

    setQuery(q);
}

void MorphologicalAnalysisModel::setTextFormWritingSystem( const WritingSystem & ws )
{
    mTextFormWs = ws;
    setLexicalEntry(mLexicalEntryId);
}

void MorphologicalAnalysisModel::setGlossWritingSystem( const WritingSystem & ws )
{
    mGlossWs = ws;
    setLexicalEntry(mLexicalEntryId);
}
