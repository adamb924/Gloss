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
    QSqlQuery q(QSqlDatabase::database( mDbAdapter->dbFilename() ));
    q.exec(mQueryString);
    setQuery(q);
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
        Type type;
        int i;
        typeFromColumn( modelIndex.column() , type , i );

        switch( type )
        {
        case LexiconModel::CitationForm:
            mDbAdapter->updateLexicalEntryCitationForm( TextBit( value.toString(), WritingSystem(), index( modelIndex.row(), modelIndex.column() - 1 ).data().toLongLong() ) );
            break;
        case LexiconModel::Gloss:
            mDbAdapter->updateLexicalEntryGloss( TextBit( value.toString(), WritingSystem(), index( modelIndex.row(), modelIndex.column() - 1 ).data().toLongLong() ) );
            break;
        case LexiconModel::Other:
            break;
        }
        emit dataChanged(modelIndex, modelIndex);
        refreshQuery();
        return true;
    }
    return false;
}

void LexiconModel::typeFromColumn( const int col, Type & type , int & index )
{
    if( col < 1 + 2*mCitationFormFields.count() )
    {
        type = LexiconModel::CitationForm;
        index = (col - 1)/2;
    }
    else if ( col < 1 + 2 * (mCitationFormFields.count() + mGlossFields.count() ) )
    {
        type = LexiconModel::Gloss;
        index = (col - mCitationFormFields.count() - 1) / 2;
    }
    else
    {
        type = LexiconModel::Other;
        index = -1;
    }
}
