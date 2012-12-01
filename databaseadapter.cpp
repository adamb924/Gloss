#include "databaseadapter.h"

#include <QMessageBox>

#include "textbit.h"
#include "writingsystem.h"

DatabaseAdapter::DatabaseAdapter(const QString & filename, QObject *parent) :
        QObject(parent)
{
    mDb = QSqlDatabase::addDatabase("QSQLITE");
    mDb.setDatabaseName(filename);
    if(!mDb.open())
    {
        QMessageBox::information (0,"Error Message","There was a problem in opening the database. The program said: " + mDb.lastError().databaseText() + " It is unlikely that you will solve this on your own. Rather you had better contact the developer." );
        return;
    }
}

DatabaseAdapter::~DatabaseAdapter()
{
    QString connection;
    connection = mDb.connectionName();
    mDb.close();

    QSqlDatabase::removeDatabase(connection);
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
    QString query = QString("select Interpretations._id from Interpretations,TextForms,WritingSystems where Form='%1' and WritingSystem=WritingSystems._id and WritingSystems.FlexString='%2' and TextForms.InterpretationId=Interpretations._id;").arg(bit.text()).arg(bit.writingSystem()->flexString());
    if( !q.exec(query)  )
        qDebug() << "DatabaseAdapter::candidateInterpretations" << q.lastError().text() << query;
    while( q.next() )
        candidates << q.value(0).toLongLong();

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
        qDebug() << "DatabaseAdapter::newInterpretationFromOrthography" << q.lastError().text() << query;
        mDb.rollback();
        return -1;
    }
    mDb.commit();
    return id;
}

qlonglong DatabaseAdapter::newInterpretation(const QList<TextBit> & textForms , const QList<TextBit> & glossForms )
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

        // TODO fix this error handling
        for(int i=0; i<textForms.count(); i++)
        {
            q.exec(QString("insert into TextForms (InterpretationId,WritingSystem,Form) values ('%1','%2','%3');").arg(id).arg(textForms.at(i).writingSystem()->id()).arg(textForms.at(i).text()));
//            if(!q.exec(QString("insert into TextForms (InterpretationId,WritingSystem,Form) values ('%1','%2','%3');").arg(id).arg(textForms.at(i).writingSystem()->id()).arg(textForms.at(i).text())));
//            throw id;
        }
        for(int i=0; i<glossForms.count(); i++)
        {
            q.exec(QString("insert into Glosses (InterpretationId,WritingSystem,Form) values ('%1','%2','%3');").arg(id).arg(glossForms.at(i).writingSystem()->id()).arg(glossForms.at(i).text()));
            //            if(!q.exec(QString("insert into Glosses (InterpretationId,WritingSystem,Form) values ('%1','%2','%3');").arg(id).arg(glossForms.at(i).writingSystem()->id()).arg(glossForms.at(i).text())));
            //        throw id;
        }
    }
    catch(int e)
    {
        qDebug() << "DatabaseAdapter::newInterpretationFromOrthography" << q.lastError().text() << query;
        mDb.rollback();
        return -1;
    }
    mDb.commit();
    return id;
}

void DatabaseAdapter::updateInterpretationTextForm( const TextBit & bit )
{
    QSqlQuery q(mDb);
    QString query = QString("replace into TextForms (InterpretationId, WritingSystem, Form) select '%1',_id,'%2' from WritingSystems where FlexString='%3';").arg(bit.id()).arg(bit.text()).arg(bit.writingSystem()->flexString());
    if( !q.exec(query)  )
        qDebug() << "DatabaseAdapter::updateInterpretationTextForm" << q.lastError().text() << query;
}

void DatabaseAdapter::updateInterpretationGloss( const TextBit & bit )
{
    QSqlQuery q(mDb);
    QString query = QString("replace into Glosses (InterpretationId, WritingSystem, Form) select '%1',_id,'%2' from WritingSystems where FlexString='%3';").arg(bit.id()).arg(bit.text()).arg(bit.writingSystem()->flexString());
    if( !q.exec(query)  )
        qDebug() << "DatabaseAdapter::updateInterpretationGloss" << q.lastError().text() << query;
}

void DatabaseAdapter::updateInterpretationMorphologicalAnalysis( const TextBit & bit, const QString & splitString )
{
    QSqlQuery q(mDb);
    QString query = QString("replace into MorphologicalAnalyses (InterpretationId, WritingSystem, Form, SplitString) select '%1',_id,'%2','%3' from WritingSystems where FlexString='%4';").arg(bit.id()).arg(bit.text()).arg(splitString).arg(bit.writingSystem()->flexString());
    if( !q.exec(query)  )
        qDebug() << "DatabaseAdapter::updateInterpretationMorphologicalAnalysis" << q.lastError().text() << query;
}

QString DatabaseAdapter::getInterpretationGloss(qlonglong id, WritingSystem *ws) const
{
    QSqlQuery q(mDb);
    QString query = QString("select Form from Glosses where InterpretationId='%1' and WritingSystem in (select _id from WritingSystems where FlexString='%2');").arg(id).arg(ws->flexString());
    if( q.exec(query) )
    {
        if( q.first() )
            return q.value(0).toString();
        else
            return "";
    }
    else
    {
        qDebug() << "DatabaseAdapter::getInterpretationGloss" << q.lastError().text() << query;
        return "";
    }
}

QString DatabaseAdapter::getInterpretationTextForm(qlonglong id, WritingSystem *ws) const
{
    QSqlQuery q(mDb);
    QString query = QString("select Form from TextForms where InterpretationId='%1' and WritingSystem in (select _id from WritingSystems where FlexString='%2');").arg(id).arg(ws->flexString());
    if( q.exec(query) )
    {
        if( q.first() )
            return q.value(0).toString();
        else
            return "";
    }
    else
    {
        qDebug() << "DatabaseAdapter::getInterpretationTextForm" << q.lastError().text() << query;
        return "";
    }
}

QString DatabaseAdapter::getInterpretationMorphologicalAnalysis(qlonglong id, WritingSystem *ws) const
{
    QSqlQuery q(mDb);
    QString query = QString("select Form from MorphologicalAnalyses where InterpretationId='%1' and WritingSystem in (select _id from WritingSystems where FlexString='%2');").arg(id).arg(ws->flexString());
    if( q.exec(query) )
    {
        if( q.first() )
            return q.value(0).toString();
        else
            return "";
    }
    else
    {
        qDebug() << "DatabaseAdapter::getInterpretationMorphologicalAnalysis" << q.lastError().text() << query;
        return "";
    }
}

QSqlDatabase* DatabaseAdapter::db()
{
    return &mDb;
}

void DatabaseAdapter::createTables()
{
    QSqlQuery q(mDb);

    if( !q.exec("create table if not exists Interpretations ( _id integer primary key autoincrement );") )
        qDebug() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists TextForms ( _id integer primary key autoincrement, InterpretationId integer, WritingSystem integer, Form text, unique (InterpretationId,WritingSystem)  );") )
        qDebug() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists MorphologicalAnalyses ( _id integer primary key autoincrement, InterpretationId integer, WritingSystem integer, Form text, SplitString text, unique (InterpretationId,WritingSystem)  );") )
        qDebug() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists Glosses ( _id integer primary key autoincrement, InterpretationId integer, WritingSystem integer, Form text, unique (InterpretationId,WritingSystem)  );") )
        qDebug() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists WritingSystems ( _id integer primary key autoincrement, Name text, Abbreviation text, FlexString text, KeyboardCommand text, Direction integer, FontFamily text, FontSize text );") )
        qDebug() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists GlossLines ( Type text, DisplayOrder integer, WritingSystem integer );") )
        qDebug() << q.lastError().text() << q.lastQuery();

    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('Persian','Prs', 'prd-Arab', 'persian.ahk' , '1' , 'ScheherazadeKerned' , '16' );");
    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('Wakhi','Wak', 'wbl-Arab-AF', 'wakhi.ahk' , '1' , 'ScheherazadeKerned' , '16' );");
    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('English','Eng', 'en', 'english.ahk' , '0' , 'Times New Roman' , '12' );");
    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('IPA','IPA', 'wbl-Qaaa-AF-fonipa-x-Zipa', 'ipa.ahk' , '0' , 'Times New Roman' , '12' );");

    q.exec("insert into GlossLines ( Type, DisplayOrder, WritingSystem )  values ('Text','1', '2');");
    q.exec("insert into GlossLines ( Type, DisplayOrder, WritingSystem )  values ('Text','2', '4');");
    q.exec("insert into GlossLines ( Type, DisplayOrder, WritingSystem )  values ('Gloss','3', '1');");
    q.exec("insert into GlossLines ( Type, DisplayOrder, WritingSystem )  values ('Gloss','4', '3');");
}

WritingSystem* DatabaseAdapter::writingSystem(QString flexString)
{
    QSqlQuery q(mDb);
    QString query = QString("select _id, Name, Abbreviation, KeyboardCommand, Direction, FontFamily, FontSize from WritingSystems where FlexString='%1';").arg(flexString);
    if( !q.exec(query)  )
        qDebug() << "Project::writingSystem" << q.lastError().text() << query;
    if( q.first() )
        return new WritingSystem(q.value(0).toLongLong(), q.value(1).toString(), q.value(2).toString(), flexString, q.value(3).toString(), (Qt::LayoutDirection)q.value(4).toInt() , q.value(5).toString(), q.value(6).toInt() );
    else
        return 0;
}

QList<WritingSystem*> DatabaseAdapter::writingSystems() const
{
    QList<WritingSystem*> list;
    QSqlQuery q(mDb);
    QString query = QString("select _id, Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize from WritingSystems;");
    if( !q.exec(query)  )
        qDebug() << "Project::writingSystems" << q.lastError().text() << query;
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

QList<qlonglong> DatabaseAdapter::candidateInterpretations(const QList<TextBit> & textForms , const QList<TextBit> & glossForms )
{
    QString query;

    for(int i=0; i<textForms.count(); i++)
    {
        query.append( QString("select distinct(InterpretationId) from TextForms where (WritingSystem='%1' and Form='%2') or InterpretationId not in (select InterpretationId from TextForms where WritingSystem= '%1') ").arg(textForms.at(i).writingSystem()->id()).arg(textForms.at(i).text()) );
        if( i != textForms.count() - 1 )
            query.append(" intersect ");
    }
    if( textForms.count() > 0 )
        query.append(" intersect ");
    for(int i=0; i<glossForms.count(); i++)
    {
        query.append( QString("select distinct(InterpretationId) from Glosses where (WritingSystem='%1' and Form='%2') or InterpretationId not in (select InterpretationId from Glosses where WritingSystem= '%1') ").arg(glossForms.at(i).writingSystem()->id()).arg(glossForms.at(i).text()) );
        if( i != glossForms.count() - 1 )
            query.append(" intersect ");
    }
    query.append(";");

    QSqlQuery q(mDb);
    q.exec(query);

    QList<qlonglong> candidates;
    while( q.next() )
        candidates << q.value(0).toLongLong();

    return candidates;
}
