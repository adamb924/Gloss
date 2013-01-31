#include "lexiconstandardmodel.h"

#include "databaseadapter.h"

LexiconStandardModel::LexiconStandardModel(const DatabaseAdapter * dbAdapter, QObject *parent) :
        QAbstractTableModel(parent)
{
    mDbAdapter = dbAdapter;
    mGlossFields= mDbAdapter->lexicalEntryGlossFields();
    mCitationFormFields = mDbAdapter->lexicalEntryCitationFormFields();

    setHeaders();

    mQuery = QSqlQuery(QSqlDatabase::database( mDbAdapter->dbFilename() ));

    mRowCount = 0;
    if( mQuery.exec( buildCountQueryString() )  && mQuery.next() )
        mRowCount = mQuery.value(0).toLongLong();

    mQueryString = buildQueryString();

    refreshQuery();
}

void LexiconStandardModel::refreshQuery()
{
    mQuery.exec( mQueryString );
}

void LexiconStandardModel::setHeaders()
{
    for(int i=0; i< mCitationFormFields.count(); i++)
        setHeaderData( columnForCitationForm( i ) , Qt::Horizontal , mCitationFormFields.at(i).name() );
    for(int i=0; i< mGlossFields.count(); i++)
        setHeaderData( columnForGloss( i ) , Qt::Horizontal , mGlossFields.at(i).name() );
}

QString LexiconStandardModel::buildQueryString() const
{
    QStringList selectStatements;
    QStringList joins;
    QStringList onStatement;

    QListIterator<WritingSystem> cIter(mCitationFormFields);
    while( cIter.hasNext() )
    {
        qlonglong wsId = cIter.next().id();

        if( selectStatements.isEmpty() )
            selectStatements << citationFormTable(wsId) + ".LexicalEntryId";
        selectStatements << citationFormTable(wsId) + "._id" << citationFormTable(wsId) + ".Form";

        joins << QString("( select Form, LexicalEntryId, _id from LexicalEntryCitationForm where WritingSystem=%1 ) as ").arg(wsId) + citationFormTable(wsId);

        if( onStatement.isEmpty() ) // the first
            onStatement << citationFormTable(wsId) + ".LexicalEntryId";
        else // not the first
            onStatement << citationFormTable(wsId) + ".LexicalEntryId and " + citationFormTable(wsId) + ".LexicalEntryId";
    }

    QListIterator<WritingSystem> gIter(mGlossFields);
    while( gIter.hasNext() )
    {
        qlonglong wsId = gIter.next().id();
        selectStatements << glossTable(wsId) + "._id" << glossTable(wsId) + ".Form";
        joins << QString("( select Form, LexicalEntryId , _id from LexicalEntryGloss where WritingSystem=%1 ) as ").arg(wsId) + glossTable(wsId);

        if( onStatement.isEmpty() ) // the first
            onStatement << glossTable(wsId) + ".LexicalEntryId";
        else // not the first
            onStatement << glossTable(wsId) + ".LexicalEntryId and " + glossTable(wsId) + ".LexicalEntryId";
    }

    // add the allomorph count
    selectStatements << "AC";
    joins << "( select LexicalEntryId, count(_id) as AC from Allomorph group by LexicalEntryId ) as AllomorphCount ";
    onStatement << "AllomorphCount.LexicalEntryId and AllomorphCount.LexicalEntryId";

    // add the text form count
    selectStatements << "TF";
    joins << "( select LexicalEntryId, count( distinct TextFormId ) as TF from MorphologicalAnalysisMembers,Allomorph where MorphologicalAnalysisMembers.AllomorphId=Allomorph._id group by LexicalEntryId ) as TextFormCount ";
    onStatement << "TextFormCount.LexicalEntryId";

    return "select " + selectStatements.join(",") + " from " + joins.join(" inner join ") + " on " + onStatement.join("=");
}

QString LexiconStandardModel::buildCountQueryString() const
{
    QStringList selects;
    QListIterator<WritingSystem> cIter(mCitationFormFields);
    while( cIter.hasNext() )
        selects << QString("select LexicalEntryId from LexicalEntryCitationForm where WritingSystem=%1").arg( cIter.next().id() );

    QListIterator<WritingSystem> gIter(mGlossFields);
    while( gIter.hasNext() )
        selects << QString("select LexicalEntryId from LexicalEntryGloss where WritingSystem=%1").arg( gIter.next().id() );

    selects << "select distinct LexicalEntryId from MorphologicalAnalysisMembers,Allomorph where MorphologicalAnalysisMembers.AllomorphId=Allomorph._id ";

    return "select count(LexicalEntryId) from ( " + selects.join(" intersect ") + ");";
}

int LexiconStandardModel::columnForCitationForm( const int index ) const
{
    return index;
}

int LexiconStandardModel::columnForGloss( const int index ) const
{
    return mCitationFormFields.count() + index;
}

int LexiconStandardModel::rowCount ( const QModelIndex & parent ) const
{
    return mRowCount;
}

void LexiconStandardModel::typeFromColumn( const int col, Type & type , int & index )
{
    if( col < mCitationFormFields.count() )
    {
        type = LexiconStandardModel::CitationForm;
        index = col;
    }
    else if ( col < mCitationFormFields.count() + mGlossFields.count() )
    {
        type = LexiconStandardModel::Gloss;
        index = col - mCitationFormFields.count();
    }
    else
    {
        type = LexiconStandardModel::Other;
        index = -1;
    }
}

int LexiconStandardModel::columnCount ( const QModelIndex & parent ) const
{
    // plus 1 for the allomorph count
    // plus 1 for the text form count
    return mCitationFormFields.count() + mGlossFields.count() + 2;
}

QVariant LexiconStandardModel::data ( const QModelIndex & index, int role ) const
{

    if( role == Qt::DisplayRole || role == Qt::EditRole )
    {
        QSqlQuery notConst = mQuery;
        notConst.seek( index.row() );

        int col;
        if( index.column() < mCitationFormFields.count() + mGlossFields.count() )
            col = 2 + 2*index.column();
        else
            col = index.column() + mCitationFormFields.count() + mGlossFields.count() + 1;

        return notConst.value( col );
    }
    else if( role == LexiconStandardModel::LexicalEntryId )
    {
        QSqlQuery notConst = mQuery;
        notConst.seek( index.row() );
        return notConst.value( 0 );
    }
    else if( role == LexiconStandardModel::TextFormGlossId )
    {
        if( index.column() < mCitationFormFields.count() + mGlossFields.count() )
        {
            QSqlQuery notConst = mQuery;
            notConst.seek( index.row() );
            return notConst.value( 1 + 2*index.column() );
        }
        else
        {
            return -1;
        }
    }

    return QVariant();
}

Qt::ItemFlags LexiconStandardModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    if( index.column() < mCitationFormFields.count() + mGlossFields.count() )
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    else
        return QAbstractItemModel::flags(index);
}

bool LexiconStandardModel::setData(const QModelIndex &index,
                                   const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        Type type;
        int i;
        typeFromColumn( index.column() , type , i );

        switch( type )
        {
        case LexiconStandardModel::CitationForm:
            mDbAdapter->updateLexicalEntryCitationForm( TextBit( value.toString(), WritingSystem(), index.data(LexiconStandardModel::TextFormGlossId).toLongLong() ) );
            break;
        case LexiconStandardModel::Gloss:
            mDbAdapter->updateLexicalEntryGloss( TextBit( value.toString(), WritingSystem(), index.data(LexiconStandardModel::TextFormGlossId).toLongLong() ) );
            break;
        case LexiconStandardModel::Other:
            break;
        }
        emit dataChanged(index, index);
        return true;
    }
    return false;
}
