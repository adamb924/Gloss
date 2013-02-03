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

    //    q.prepare("select Focus.TextFormId as ID, Focus, TextForm, Gloss from (select TextFormId,group_concat( Transcription , ' ' ) as TextForm, group_concat( Gloss , '-' ) as Gloss from (select TextFormId,AllomorphId,Allomorph.Form as Transcription,LexicalEntryGloss.Form as Gloss from MorphologicalAnalysisMembers,Allomorph,LexicalEntryGloss where TextFormId in ( select TextFormId from MorphologicalAnalysisMembers where AllomorphId in (select _id from Allomorph where  LexicalEntryId=:LexicalEntryId and WritingSystem=:TextFormWS) ) and AllomorphId = Allomorph._id and Allomorph.LexicalEntryId = LexicalEntryGloss.LexicalEntryId and LexicalEntryGloss.WritingSystem=:GlossWS order by TextFormId, AllomorphOrder) group by TextFormId ) as Concatenation left join  ( select TextFormId, Form as Focus from Allomorph,MorphologicalAnalysisMembers on Allomorph._id=MorphologicalAnalysisMembers.AllomorphId and LexicalEntryId=:LexicalEntryId ) as Focus on Focus.TextFormId = Concatenation.TextFormId;");
    //    q.bindValue(":LexicalEntryId", mLexicalEntryId);
    //    q.bindValue(":GlossWS", mGlossWs.id());
    //    q.bindValue(":TextFormWS", mTextFormWs.id());

    q.prepare(
            QString("select Focus.TextFormId as ID, Focus, TextForm, Gloss from (select TextFormId,group_concat( Transcription , ' ' ) as TextForm, group_concat( Gloss , '-' ) as Gloss from (select TextFormId,AllomorphId,Allomorph.Form as Transcription,LexicalEntryGloss.Form as Gloss from MorphologicalAnalysisMembers,Allomorph,LexicalEntryGloss where TextFormId in ( select TextFormId from MorphologicalAnalysisMembers where AllomorphId in (select _id from Allomorph where  LexicalEntryId=%1 and WritingSystem=%3) ) and AllomorphId = Allomorph._id and Allomorph.LexicalEntryId = LexicalEntryGloss.LexicalEntryId and LexicalEntryGloss.WritingSystem=%2 order by TextFormId, AllomorphOrder) group by TextFormId ) as Concatenation left join  ( select TextFormId, Form as Focus from Allomorph,MorphologicalAnalysisMembers on Allomorph._id=MorphologicalAnalysisMembers.AllomorphId and LexicalEntryId=%1 ) as Focus on Focus.TextFormId = Concatenation.TextFormId;")
            .arg(mLexicalEntryId).arg(mGlossWs.id()).arg(mTextFormWs.id())
            );

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
