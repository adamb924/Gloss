#include "lexiconmodel.h"

#include <QListIterator>
#include <QSqlQuery>

#include "databaseadapter.h"

LexiconModel::LexiconModel(const DatabaseAdapter * dbAdapter, QObject *parent) :
    QSqlQueryModel(parent)
{
    mDbAdapter = dbAdapter;
    mGlossFields= mDbAdapter->lexicalEntryGlossFields();
    mCitationFormFields = mDbAdapter->lexicalEntryCitationFormFields();

    mQueryString = buildQueryString();

    refreshQuery();
}

void LexiconModel::refreshQuery()
{
    setQuery(mQueryString , QSqlDatabase::database( mDbAdapter->dbFilename() ) );
}

QString LexiconModel::buildQueryString()
{
    QStringList selectStatements;
    QStringList joins;
    QStringList onStatement;

    QListIterator<WritingSystem> cIter(mCitationFormFields);
    while( cIter.hasNext() )
    {
        WritingSystem ws = cIter.next();
        qlonglong wsId = ws.id();

        if( selectStatements.isEmpty() )
        {
            selectStatements << citationFormTable(wsId) + ".LexicalEntryId as ID";
            mEditable << false;
        }
        selectStatements << citationFormTable(wsId) + "._id" << citationFormTable(wsId) + QString(".Form as '%1'").arg(ws.name());
        mEditable << false << true;

        joins << QString("( select Form, LexicalEntryId, _id from LexicalEntryCitationForm where WritingSystem=%1 ) as ").arg(wsId) + citationFormTable(wsId);

        if( onStatement.isEmpty() ) // the first
            onStatement << citationFormTable(wsId) + ".LexicalEntryId";
        else // not the first
            onStatement << citationFormTable(wsId) + ".LexicalEntryId and " + citationFormTable(wsId) + ".LexicalEntryId";
    }

    QListIterator<WritingSystem> gIter(mGlossFields);
    while( gIter.hasNext() )
    {
        WritingSystem ws = gIter.next();
        qlonglong wsId = ws.id();

        selectStatements << glossTable(wsId) + "._id" << glossTable(wsId) + QString(".Form as '%1'").arg(ws.name());
        mEditable << false << true;

        joins << QString("( select Form, LexicalEntryId , _id from LexicalEntryGloss where WritingSystem=%1 ) as ").arg(wsId) + glossTable(wsId);

        if( onStatement.isEmpty() ) // the first
            onStatement << glossTable(wsId) + ".LexicalEntryId";
        else // not the first
            onStatement << glossTable(wsId) + ".LexicalEntryId and " + glossTable(wsId) + ".LexicalEntryId";
    }

    // add the morpheme type
    selectStatements << "MorphologicalCategory as 'Type'";
    mEditable << true;
    joins << "( select _id as LexicalEntryId, MorphologicalCategory from LexicalEntry ) as MC ";
    onStatement << "MC.LexicalEntryId and MC.LexicalEntryId";

    // add the allomorph count
    selectStatements << "AC as '# Allomorphs'";
    mEditable << false;
    joins << "( select LexicalEntryId, count(_id) as AC from Allomorph group by LexicalEntryId ) as AllomorphCount ";
    onStatement << "AllomorphCount.LexicalEntryId and AllomorphCount.LexicalEntryId";

    // add the text form count
    selectStatements << "TF as '# Text Forms'";
    mEditable << false;
    joins << "( select LexicalEntryId, count( distinct TextFormId ) as TF from MorphologicalAnalysisMembers,Allomorph where MorphologicalAnalysisMembers.AllomorphId=Allomorph._id group by LexicalEntryId ) as TextFormCount ";
    onStatement << "TextFormCount.LexicalEntryId and TextFormCount.LexicalEntryId";

    return "select " + selectStatements.join(",") + " from " + joins.join(" inner join ") + " on " + onStatement.join("=");
}

qlonglong LexiconModel::lexicalEntryId( const QModelIndex & modelIndex ) const
{
    return index( modelIndex.row() , 0 ).data().toLongLong();
}

QVariant LexiconModel::data ( const QModelIndex & index, int role ) const
{
    if( role == Qt::UserRole )
    {
        return lexicalEntryId( index );
    }
    else
    {
        return QSqlQueryModel::data( index, role );
    }
}

Qt::ItemFlags LexiconModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    if( mEditable.at(index.column()) )
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    else
        return QAbstractItemModel::flags(index);
}

bool LexiconModel::setData(const QModelIndex &modelIndex,
                                   const QVariant &value, int role)
{
    if (modelIndex.isValid() && role == Qt::EditRole)
    {
        // the database index can be the _id of different tables. And it has to be declared out here because of how switches work.
        qlonglong databaseIndex = -1;

        switch( typeFromColumn( modelIndex.column() ) )
        {
        case LexiconModel::CitationForm:
            databaseIndex = index( modelIndex.row(), modelIndex.column() - 1 ).data().toLongLong();
            clear();
            mDbAdapter->updateLexicalEntryCitationForm( TextBit( value.toString(), WritingSystem(), databaseIndex ) );
            break;
        case LexiconModel::Gloss:
            databaseIndex = index( modelIndex.row(), modelIndex.column() - 1 ).data().toLongLong();
            clear();
            mDbAdapter->updateLexicalEntryGloss( TextBit( value.toString(), WritingSystem(), databaseIndex ) );
            break;
        case LexiconModel::MorphologicalType:
            databaseIndex = lexicalEntryId( modelIndex );
            clear();
            if( Allomorph::getType( value.toString() ) != Allomorph::Null )
                mDbAdapter->updateLexicalEntryType( databaseIndex , value.toString() );
            break;
        case LexiconModel::Other:
            break;
        }
        refreshQuery();
        return true;
    }
    return false;
}

LexiconModel::Type LexiconModel::typeFromColumn(int col)
{
    if( col < 1 + 2*mCitationFormFields.count() )
    {
        return LexiconModel::CitationForm;
    }
    else if ( col < 1 + 2 * (mCitationFormFields.count() + mGlossFields.count() ) )
    {
        return LexiconModel::Gloss;
    }
    else if ( col == 1 + 2 * (mCitationFormFields.count() + mGlossFields.count() ) )
    {
        return LexiconModel::MorphologicalType;
    }
    else
    {
        return LexiconModel::Other;
    }
}

WritingSystem LexiconModel::writingSystemFromColumn(int col)
{
    int visibleColumn = (col-1)/2; // actually, the index of the visible column, after the Id column
    if( visibleColumn < mCitationFormFields.count() )
        return mCitationFormFields.at(visibleColumn);
    visibleColumn -= mCitationFormFields.count();
    if( visibleColumn < mGlossFields.count() )
        return mGlossFields.at(visibleColumn);
    return WritingSystem();
}
