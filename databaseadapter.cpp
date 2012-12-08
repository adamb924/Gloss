#include "databaseadapter.h"

#include <QMessageBox>
#include <QHashIterator>

#include "textbit.h"
#include "writingsystem.h"

DatabaseAdapter::DatabaseAdapter(const QString & filename, QObject *parent) :
        QObject(parent)
{
    mFilename = filename;
    mDb = QSqlDatabase::addDatabase("QSQLITE");
    mDb.setHostName("hostname");
    mDb.setDatabaseName(filename);
    if(!mDb.open())
    {
        QMessageBox::information (0,"Error Message","There was a problem in opening the database. The program said: " + mDb.lastError().databaseText() + " It is unlikely that you will solve this on your own. Rather you had better contact the developer." );
        return;
    }
}

DatabaseAdapter::~DatabaseAdapter()
{
//    QString connection;
//    connection = mDb.connectionName();
//    mDb.close();

//    QSqlDatabase::removeDatabase(connection);
}

void DatabaseAdapter::initialize(QString filename)
{
    mDb.setDatabaseName(filename);
    if(!mDb.open())
    {
        QMessageBox::information (0,"Error Message","There was a problem in opening the database. The program said: " + mDb.lastError().databaseText() + " It is unlikely that you will solve this on your own. Rather you had better contact the developer." );
        return;
    }
    createTables();
}

QList<qlonglong> DatabaseAdapter::candidateInterpretations(const TextBit & bit) const
{
    QList<qlonglong> candidates;
    QSqlQuery q(mDb);
    QString query = QString("select InterpretationId from TextForms where Form='%1' and WritingSystem='%2';").arg(bit.text()).arg(bit.writingSystem().id());
    if( !q.exec(query)  )
        qWarning() << "DatabaseAdapter::candidateInterpretations" << q.lastError().text() << query;
    while( q.next() )
        candidates << q.value(0).toLongLong();

    return candidates;
}

bool DatabaseAdapter::hasMultipleCandidateInterpretations(const TextBit & bit) const
{
    QSqlQuery q(mDb);
    QString query = QString("select count(distinct InterpretationId) from TextForms where Form='%1' and WritingSystem='%2';").arg(bit.text()).arg(bit.writingSystem().id());
    if( !q.exec(query)  )
        qWarning() << "DatabaseAdapter::candidateInterpretations" << q.lastError().text() << query;
    if( q.next() )
        return q.value(0).toInt() > 1;
    else
        return false;
}

QHash<qlonglong,QString> DatabaseAdapter::candidateInterpretationWithSummaries(const TextBit & bit) const
{
    QHash<qlonglong,QString> candidates;
    QSqlQuery q(mDb);
    QString query = QString("select InterpretationId,group_concat(Form,', ') from ( select InterpretationId, Form from TextForms where InterpretationId in (select InterpretationId from TextForms where Form='%1' and WritingSystem='%2') union select InterpretationId, Form from Glosses where InterpretationId in (select InterpretationId from TextForms where Form='%1' and WritingSystem='%2') ) group by InterpretationId;").arg(bit.text()).arg(bit.writingSystem().id());
    if( !q.exec(query)  )
        qWarning() << "DatabaseAdapter::candidateInterpretations" << q.lastError().text() << query;
    while( q.next() )
        candidates.insert( q.value(0).toLongLong() , q.value(1).toString() );
    return candidates;
}

QHash<qlonglong,QString> DatabaseAdapter::interpretationTextForms(qlonglong interpretationId, qlonglong writingSystemId) const
{
    QHash<qlonglong,QString> candidates;
    QSqlQuery q(mDb);
    QString query = QString("select _id,Form from TextForms where InterpretationId='%1' and WritingSystem='%2';").arg(interpretationId).arg(writingSystemId);
    if( !q.exec(query)  )
        qWarning() << "DatabaseAdapter::interpretationTextForms" << q.lastError().text() << query;
    while( q.next() )
        candidates.insert( q.value(0).toLongLong() , q.value(1).toString() );
    return candidates;
}

QHash<qlonglong,QString> DatabaseAdapter::interpretationGlosses(qlonglong interpretationId, qlonglong writingSystemId) const
{
    QHash<qlonglong,QString> candidates;
    QSqlQuery q(mDb);
    QString query = QString("select _id,Form from Glosses where InterpretationId='%1' and WritingSystem='%2';").arg(interpretationId).arg(writingSystemId);
    if( !q.exec(query)  )
        qWarning() << "DatabaseAdapter::interpretationGlosses" << q.lastError().text() << query;
    while( q.next() )
        candidates.insert( q.value(0).toLongLong() , q.value(1).toString() );
    return candidates;
}

qlonglong DatabaseAdapter::newInterpretation( const TextBit & bit )
{
    mDb.transaction();

    QSqlQuery q(mDb);
    QString query;
    qlonglong id;

    try
    {
        if(!q.exec("insert into Interpretations (_id) select null;"))
            throw;
        id = q.lastInsertId().toLongLong();

        TextBit newBit(bit);
        newBit.setId(id);
        updateInterpretationTextForm(newBit);
    }
    catch(std::exception &e)
    {
        qWarning() << "DatabaseAdapter::newInterpretationFromOrthography" << q.lastError().text() << query;
        mDb.rollback();
        return -1;
    }
    mDb.commit();
    return id;
}

qlonglong DatabaseAdapter::newTextForm(qlonglong interpretationId, qlonglong writingSystemId)
{
    QSqlQuery q(mDb);
    QString query = QString("insert into TextForms (InterpretationId,WritingSystem) values ('%1','%2');").arg(interpretationId).arg(writingSystemId);
    if(q.exec( query ))
    {
        return q.lastInsertId().toLongLong();
    }
    else
    {
        qWarning() << "DatabaseAdapter::newTextForm" << q.lastError().text() << query;
        return -1;
    }
}

qlonglong DatabaseAdapter::newGloss(qlonglong interpretationId, qlonglong writingSystemId)
{
    QSqlQuery q(mDb);
    QString query = QString("insert into Glosses (InterpretationId,WritingSystem) values ('%1','%2');").arg(interpretationId).arg(writingSystemId);
    if(q.exec(query) )
    {
        return q.lastInsertId().toLongLong();
    }
    else
    {
        qWarning() << "DatabaseAdapter::newGloss" << q.lastError().text() << query;
        return -1;
    }
}

qlonglong DatabaseAdapter::newInterpretation( TextBitHash & textForms , TextBitHash & glossForms )
{
    mDb.transaction();

    QSqlQuery q(mDb);
    QString query;
    qlonglong id;

    try
    {
        if(!q.exec("insert into Interpretations (_id) select null;"))
            throw -1;
        id = q.lastInsertId().toLongLong();

        // TODO do some error handling

        TextBitMutableHashIterator textIter(textForms);
        while (textIter.hasNext())
        {
            textIter.next();
            q.exec(QString("insert into TextForms (InterpretationId,WritingSystem,Form) values ('%1','%2','%3');").arg(id).arg( textIter.key().id() ).arg( textIter.value().text() ));
            textIter.value().setId( q.lastInsertId().toLongLong() );
        }

        textIter = TextBitMutableHashIterator(glossForms);
        while (textIter.hasNext())
        {
            textIter.next();
            q.exec(QString("insert into Glosses (InterpretationId,WritingSystem,Form) values ('%1','%2','%3');").arg(id).arg( textIter.key().id() ).arg( textIter.value().text() ));
            textIter.value().setId( q.lastInsertId().toLongLong() );
        }
    }
    catch(int e)
    {
        qWarning() << "DatabaseAdapter::newInterpretationFromOrthography" << q.lastError().text() << query;
        mDb.rollback();
        return -1;
    }
    mDb.commit();
    return id;
}

void DatabaseAdapter::updateInterpretationTextForm( const TextBit & bit )
{
    QSqlQuery q(mDb);
    QString query = QString("update TextForms set Form='%1' where _id='%2';").arg( bit.text() ).arg( bit.id() );
    if( !q.exec(query)  )
        qWarning() << "DatabaseAdapter::updateInterpretationTextForm" << q.lastError().text() << query;
}

void DatabaseAdapter::updateInterpretationGloss( const TextBit & bit )
{
    QSqlQuery q(mDb);
    QString query = QString("update Glosses set Form='%1' where _id='%2';").arg(bit.text()).arg(bit.id());
    if( !q.exec(query)  )
        qWarning() << "DatabaseAdapter::updateInterpretationGloss" << q.lastError().text() << query;
}

void DatabaseAdapter::updateInterpretationMorphologicalAnalysis( const TextBit & bit, const QString & splitString )
{
    // TODO implement this eventually
}

TextBitHash DatabaseAdapter::getInterpretationGlosses(qlonglong id) const
{
    TextBitHash glosses;
    QSqlQuery q(mDb);
    QString query = QString("select Form,FlexString,Glosses._id from Glosses,WritingSystems where InterpretationId='%1' and WritingSystem=WritingSystems._id;").arg(id);
    if( q.exec(query) )
        while( q.next() )
            glosses.insert( writingSystem( q.value(1).toString() ) , TextBit( q.value(0).toString() , writingSystem( q.value(1).toString() ) , q.value(2).toLongLong() ) );
    return glosses;
}

TextBitHash DatabaseAdapter::getInterpretationTextForms(qlonglong id) const
{
    TextBitHash textForms;
    QSqlQuery q(mDb);
    QString query = QString("select Form,FlexString,TextForms._id from TextForms,WritingSystems where InterpretationId='%1' and WritingSystem=WritingSystems._id;").arg(id);
    if( q.exec(query) )
        while( q.next() )
            textForms.insert( writingSystem( q.value(1).toString() ) , TextBit( q.value(0).toString() , writingSystem( q.value(1).toString() ) , q.value(2).toLongLong() ) );
    return textForms;
}

QSqlDatabase* DatabaseAdapter::db()
{
    return &mDb;
}

void DatabaseAdapter::createTables()
{
    QSqlQuery q(mDb);

    if( !q.exec("create table if not exists Interpretations ( _id integer primary key autoincrement );") )
        qWarning() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists TextForms ( _id integer primary key autoincrement, InterpretationId integer, WritingSystem integer, Form text );") )
        qWarning() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists MorphologicalAnalyses ( _id integer primary key autoincrement, InterpretationId integer, WritingSystem integer, Form text, SplitString text );") )
        qWarning() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists Glosses ( _id integer primary key autoincrement, InterpretationId integer, WritingSystem integer, Form text );") )
        qWarning() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists WritingSystems ( _id integer primary key autoincrement, Name text, Abbreviation text, FlexString text, KeyboardCommand text, Direction integer, FontFamily text, FontSize text );") )
        qWarning() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists GlossLines ( Type text, DisplayOrder integer, WritingSystem integer );") )
        qWarning() << q.lastError().text() << q.lastQuery();

    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('Persian','Prs', 'prd-Arab', 'persian.ahk' , '1' , 'Scheherazade' , '16' );");
    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('Wakhi','Wak', 'wbl-Arab-AF', 'wakhi.ahk' , '1' , 'Scheherazade' , '16' );");
    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('English','Eng', 'en', 'english.ahk' , '0' , 'Times New Roman' , '12' );");
    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('IPA','IPA', 'wbl-Qaaa-AF-fonipa-x-Zipa', 'ipa.ahk' , '0' , 'Times New Roman' , '12' );");

//    q.exec("insert into GlossLines ( Type, DisplayOrder, WritingSystem )  values ('Text','1', '2');");
    q.exec("insert into GlossLines ( Type, DisplayOrder, WritingSystem )  values ('Text','2', '4');");
    q.exec("insert into GlossLines ( Type, DisplayOrder, WritingSystem )  values ('Gloss','3', '1');");
    q.exec("insert into GlossLines ( Type, DisplayOrder, WritingSystem )  values ('Gloss','4', '3');");
}

WritingSystem DatabaseAdapter::writingSystem(const QString & flexString) const
{
    QSqlQuery q(mDb);
    QString query = QString("select _id, Name, Abbreviation, KeyboardCommand, Direction, FontFamily, FontSize from WritingSystems where FlexString='%1';").arg(flexString);
    if( !q.exec(query)  )
        qWarning() << "DatabaseAdapter::writingSystem" << q.lastError().text() << query;
    if( q.first() )
        return WritingSystem(q.value(0).toLongLong(), q.value(1).toString(), q.value(2).toString(), flexString, q.value(3).toString(), (Qt::LayoutDirection)q.value(4).toInt() , q.value(5).toString(), q.value(6).toInt() );
    else
        return WritingSystem();
}

WritingSystem DatabaseAdapter::writingSystem(qlonglong id) const
{
    QSqlQuery q(mDb);
    QString query = QString("select _id, Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize from WritingSystems where _id='%1';").arg(id);
    if( !q.exec(query)  )
        qWarning() << "DatabaseAdapter::writingSystem" << q.lastError().text() << query;
    if( q.first() )
        return WritingSystem(q.value(0).toLongLong(), q.value(1).toString(), q.value(2).toString(), q.value(3).toString(), q.value(4).toString(), (Qt::LayoutDirection)q.value(5).toInt() , q.value(6).toString(), q.value(7).toInt() );
    else
        return WritingSystem();
}

QList<WritingSystem*> DatabaseAdapter::writingSystems() const
{
    QList<WritingSystem*> list;
    QSqlQuery q(mDb);
    QString query = QString("select _id, Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize from WritingSystems;");
    if( !q.exec(query)  )
        qWarning() << "DatabaseAdapter::writingSystems" << q.lastError().text() << query;
    while( q.next() )
        list << new WritingSystem(q.value(0).toLongLong(), q.value(1).toString(), q.value(2).toString(), q.value(3).toString(), q.value(4).toString(), (Qt::LayoutDirection)q.value(5).toInt() , q.value(6).toString(), q.value(7).toInt() );
    return list;
}

bool DatabaseAdapter::writingSystemExists(const QString & flexstring) const
{
    QSqlQuery q(mDb);
    QString query = QString("select FlexString from WritingSystems where FlexString='%1';").arg(flexstring);
    q.exec(query);
    return q.first();
}

void DatabaseAdapter::addWritingSystem(const QString & flexString, const QString & fontFamily, Qt::LayoutDirection layoutDirection)
{
    QSqlQuery q(mDb);
    q.exec(QString("insert into WritingSystems ( FlexString, FontFamily, Direction )  values ( '%1' , '%2' , '%3' );").arg(flexString).arg(fontFamily).arg(layoutDirection));
}

QList<qlonglong> DatabaseAdapter::candidateInterpretations(const TextBitHash & textForms , const TextBitHash & glossForms )
{
    QString query;

    TextBitHashIterator textIter(textForms);
    while (textIter.hasNext())
    {
        textIter.next();

        query.append( QString("select distinct(InterpretationId) from TextForms where (WritingSystem='%1' and Form='%2') or InterpretationId not in (select InterpretationId from TextForms where WritingSystem= '%1') ").arg( textIter.key().id() ).arg( textIter.value().text() ) );
        if( textIter.hasNext() )
            query.append(" intersect ");
    }

    if( glossForms.count() > 0 )
        query.append(" intersect ");

    textIter = TextBitHashIterator(glossForms);
    while (textIter.hasNext())
    {
        textIter.next();

        query.append( QString("select distinct(InterpretationId) from Glosses where (WritingSystem='%1' and Form='%2') or InterpretationId not in (select InterpretationId from Glosses where WritingSystem= '%1') ").arg( textIter.key().id() ).arg( textIter.value().text() ) );
        if( textIter.hasNext() )
            query.append(" intersect ");
    }

    query.append(";");

    QSqlQuery q(mDb);
    if( !q.exec(query)  )
        qWarning() << "DatabaseAdapter::candidateInterpretations" << q.lastError().text() << query;

    QList<qlonglong> candidates;
    while( q.next() )
        candidates << q.value(0).toLongLong();

    return candidates;
}

void DatabaseAdapter::close()
{
    mDb.close();
    QSqlDatabase::removeDatabase(mFilename);
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

QList<GlossLine> DatabaseAdapter::glossLines() const
{
    QList<GlossLine> lines;
    QSqlQuery q(QSqlDatabase::database( "qt_sql_default_connection" ));

    QString query = QString("select Type, _id, Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize from GlossLines,WritingSystems where GlossLines.WritingSystem=WritingSystems._id order by DisplayOrder asc;");
    if( !q.exec(query)  )
        qWarning() << "DatabaseAdapter::glossLines" << q.lastError().text() << query;
    while( q.next() )
    {
        GlossLine::LineType type;
        QString sType = q.value(0).toString();
        if ( sType == "Text" )
            type = GlossLine::Text;
        else
            type = GlossLine::Gloss;
        lines << GlossLine(type, WritingSystem( q.value(1).toLongLong(), q.value(2).toString(), q.value(3).toString(), q.value(4).toString(), q.value(5).toString(), (Qt::LayoutDirection)q.value(6).toInt() , q.value(7).toString() , q.value(8).toInt() ) );
    }
    return lines;
}

TextBit DatabaseAdapter::glossFromId(qlonglong id) const
{
    QSqlQuery q(mDb);
    QString query = QString("select _id,Form,WritingSystem from Glosses where _id='%1';").arg(id);
    if( q.exec(query) && q.next() )
    {
        return TextBit( q.value(1).toString() , writingSystem( q.value(2).toLongLong() ) , q.value(0).toLongLong() );
    }
    else
    {
        qWarning() << "DatabaseAdapter::glossFromId" << q.lastError().text() << query;
        return TextBit();
    }
}

TextBit DatabaseAdapter::textFormFromId(qlonglong id) const
{
    QSqlQuery q(mDb);
    QString query = QString("select _id,Form,WritingSystem from TextForms where _id='%1';").arg(id);
    if( q.exec(query) && q.next() )
    {
        return TextBit( q.value(1).toString() , writingSystem( q.value(2).toLongLong() ) , q.value(0).toLongLong() );
    }
    else
    {
        qWarning() << "DatabaseAdapter::textFormFromId" << q.lastError().text() << query;
        return TextBit();
    }
}
