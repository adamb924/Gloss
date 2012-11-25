#include "project.h"
#include "glossline.h"
#include "textbit.h"

#include <QMessageBox>
#include <QtDebug>

Project::Project()
{
    mDb = QSqlDatabase::addDatabase("QSQLITE");

    mWritingSystems["prd-Arab"] = new WritingSystem("Persian","Prs","prd-Arab","persian.ahk",Qt::RightToLeft);
    mWritingSystems["en"] = new WritingSystem("English","Eng","en","english.ahk",Qt::LeftToRight);
    mWritingSystems["wbl-Qaaa-AF-fonipa-x-Zipa"] = new WritingSystem("IPA","IPA","wbl-Qaaa-AF-fonipa-x-Zipa","ipa.ahk",Qt::LeftToRight);
}

Project::~Project()
{
    QString connection;
    connection = mDb.connectionName();
    mDb.close();

    QSqlDatabase::removeDatabase(connection);
}

void Project::initialize(QString filename)
{
    // TODO fix this so it works
    if( QFile::exists(filename))
    {
        QFile old(filename);
        old.setPermissions(QFile::ReadOther | QFile::WriteOther);
        qDebug() << old.remove();
    }

    mDb.setDatabaseName(filename);
    if(!mDb.open())
    {
            QMessageBox::information (0,"Error Message","There was a problem in opening the database. The program said: " + mDb.lastError().databaseText() + " It is unlikely that you will solve this on your own. Rather you had better contact the developer." );
            return;
    }
    createTables();
}

void Project::readFromFile(QString filename)
{
    mDb.setDatabaseName(filename);
    if(!mDb.open())
    {
            QMessageBox::information (0,"Error Message","There was a problem in opening the database. The program said: " + mDb.lastError().databaseText() + " It is unlikely that you will solve this on your own. Rather you had better contact the developer." );
            return;
    }
}

void Project::createTables()
{
    QSqlQuery q(mDb);

    if( !q.exec("create table Interpretations ( _id integer primary key autoincrement );") )
        qDebug() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table OrthographicForms ( _id integer primary key autoincrement, InterpretationId integer, WritingSystem integer, Form text, unique (InterpretationId,WritingSystem)  );") )
        qDebug() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table PhoneticForms ( _id integer primary key autoincrement, InterpretationId integer, WritingSystem integer, Form text, unique (InterpretationId,WritingSystem)  );") )
        qDebug() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table MorphologicalAnalyses ( _id integer primary key autoincrement, InterpretationId integer, WritingSystem integer, Form text, SplitString text, unique (InterpretationId,WritingSystem)  );") )
        qDebug() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table Glosses ( _id integer primary key autoincrement, InterpretationId integer, WritingSystem integer, Form text, unique (InterpretationId,WritingSystem)  );") )
        qDebug() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table WritingSystems ( _id integer primary key autoincrement, Name text, Abbreviation text, FlexString text, KeyboardCommand text, Direction integer );") )
        qDebug() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table GlossLines ( Type text, DisplayOrder integer, WritingSystem integer );") )
        qDebug() << q.lastError().text() << q.lastQuery();

    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction )  values ('Persian','Prs', 'prd-Arab', 'persian.ahk' , '1' );");
    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction )  values ('Wakhi','Wak', 'wbl-Arab-AF', 'wakhi.ahk' , '1' );");
    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction )  values ('English','Eng', 'en', 'english.ahk' , '0' );");
    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction )  values ('IPA','IPA', 'wbl-Qaaa-AF-fonipa-x-Zipa', 'ipa.ahk' , '0' );");

    q.exec("insert into GlossLines ( Type, DisplayOrder, WritingSystem )  values ('Orthography','1', '2');");
    q.exec("insert into GlossLines ( Type, DisplayOrder, WritingSystem )  values ('Transcription','2', '4');");
    q.exec("insert into GlossLines ( Type, DisplayOrder, WritingSystem )  values ('Gloss','3', '1');");
    q.exec("insert into GlossLines ( Type, DisplayOrder, WritingSystem )  values ('Gloss','4', '3');");
}

WritingSystem* Project::writingSystem(QString code)
{
    QSqlQuery q(mDb);
    QString query = QString("select Name, Abbreviation, KeyboardCommand, Direction from WritingSystems where FlexString='%1';").arg(code);
    if( !q.exec(query)  )
        qDebug() << "Project::writingSystem" << q.lastError().text() << query;
    if( q.first() )
        return new WritingSystem(q.value(0).toString(), q.value(1).toString(), code, q.value(2).toString(), (Qt::LayoutDirection)q.value(0).toInt() );
    else
        return new WritingSystem();
}

QHash<QString, WritingSystem*>* Project::writingSystems()
{
    return &mWritingSystems;
}

QList<GlossLine> Project::glossLines()
{
    QList<GlossLine> lines;
//    QSqlQuery q(mDb);
    QSqlQuery q(QSqlDatabase::database( "qt_sql_default_connection" ));

    QString query = QString("select Type, Name, Abbreviation, FlexString, KeyboardCommand, Direction from GlossLines,WritingSystems where GlossLines.WritingSystem=WritingSystems._id order by DisplayOrder asc;");
    if( !q.exec(query)  )
        qDebug() << "Project::glossLines" << q.lastError().text() << query;
    while( q.next() )
    {
        Project::GlossLineType type = Project::Orthography; // just not to leave it uninitialized
        QString sType = q.value(0).toString();
        if( sType == "Orthography" )
            type = Project::Orthography;
        else if ( sType == "Transcription" )
            type = Project::Transcription;
        else if ( sType == "Gloss" )
            type = Project::Gloss;
        lines << GlossLine(type, new WritingSystem( q.value(1).toString(), q.value(2).toString(), q.value(3).toString(), q.value(4).toString(), (Qt::LayoutDirection)q.value(5).toInt() ) );
    }
    return lines;
}

QList<qlonglong> Project::candidateInterpretationsPhonetic(const QString & form) const
{
    QList<qlonglong> candidates;
    QSqlQuery q(mDb);
    QString query = QString("select Interpretations._id from Interpretations,PhoneticForms where Form='%1' and PhoneticForms.InterpretationId=Interpretations._id;").arg(form);
    if( !q.exec(query)  )
        qDebug() << "Project::candidateInterpretations" << q.lastError().text() << query;
    while( q.next() )
        candidates << q.value(0).toLongLong();

    return candidates;
}


QList<qlonglong> Project::candidateInterpretationsOrthographic(const QString & form) const
{
    QList<qlonglong> candidates;
    QSqlQuery q(mDb);
    QString query = QString("select Interpretations._id from Interpretations,OrthographicForms where Form='%1' and OrthographicForms.InterpretationId=Interpretations._id;").arg(form);
    if( !q.exec(query)  )
        qDebug() << "Project::candidateInterpretations" << q.lastError().text() << query;
    while( q.next() )
        candidates << q.value(0).toLongLong();

    return candidates;
}

qlonglong Project::newInterpretationFromOrthography( const TextBit & bit )
{
    mDb.transaction();

    QSqlQuery q(mDb);
    QString query;

    try
    {
        if(!q.exec("insert into Interpretations (_id) select null;"))
            throw;

        qlonglong id = q.lastInsertId().toLongLong();

        // a bit profligate, but why not?
        // this ought not to be needed because they will be inserted later anyway
/*
        // OrthographicForm
        if(!q.exec(QString("insert into OrthographicForms (InterpretationId,WritingSystem) select '%1',_id from WritingSystems;").arg(id)))
            throw;

        // PhoneticForm
        if(!q.exec(QString("insert into PhoneticForms (InterpretationId,WritingSystem) select '%1',_id from WritingSystems;").arg(id)))
            throw;

        // MorphologicalAnalysis
        if(!q.exec(QString("insert into MorphologicalAnalyses (InterpretationId,WritingSystem) select '%1',_id from WritingSystems;").arg(id)))
            throw;

        // Gloss
        if(!q.exec(QString("insert into Glosses (InterpretationId,WritingSystem) select '%1',_id from WritingSystems;").arg(id)))
            throw;
*/
        TextBit newBit(bit);
        newBit.setId(id);
        updateInterpretationOrthography(newBit);

    }
    catch(std::exception &e)
    {
        qDebug() << "Project::newInterpretationFromOrthography" << q.lastError().text() << query;
        mDb.rollback();
        return -1;
    }
    mDb.commit();
    return q.lastInsertId().toLongLong();
}

qlonglong Project::newInterpretationFromPhonetic( const TextBit & bit )
{
    // TODO implement this
    return -1;
}

void Project::updateInterpretationGloss( const TextBit & bit )
{
    qDebug() << "Project::updateInterpretationGloss";
    QSqlQuery q(mDb);
    QString query = QString("replace into Glosses (InterpretationId, WritingSystem, Form) select '%1',_id,'%2' from WritingSystems where FlexString='%3';").arg(bit.id()).arg(bit.text()).arg(bit.writingSystem()->flexString());
    if( !q.exec(query)  )
        qDebug() << "Project::updateInterpretationGloss" << q.lastError().text() << query;
}

void Project::updateInterpretationTranscription( const TextBit & bit )
{
    qDebug() << "Project::updateInterpretationTranscription";
    QSqlQuery q(mDb);
    QString query = QString("replace into PhoneticForms (InterpretationId, WritingSystem, Form) select '%1',_id,'%2' from WritingSystems where FlexString='%3';").arg(bit.id()).arg(bit.text()).arg(bit.writingSystem()->flexString());
    if( !q.exec(query)  )
        qDebug() << "Project::updateInterpretationTranscription" << q.lastError().text() << query;
}

void Project::updateInterpretationOrthography( const TextBit & bit )
{
    QSqlQuery q(mDb);
    QString query = QString("replace into OrthographicForms (InterpretationId, WritingSystem, Form) select '%1',_id,'%2' from WritingSystems where FlexString='%3';").arg(bit.id()).arg(bit.text()).arg(bit.writingSystem()->flexString());
    if( !q.exec(query)  )
        qDebug() << "Project::updateInterpretationOrthography" << q.lastError().text() << query;
}

void Project::updateInterpretationMorphologicalAnalysis( const TextBit & bit, const QString & splitString )
{
    QSqlQuery q(mDb);
    QString query = QString("replace into MorphologicalAnalyses (InterpretationId, WritingSystem, Form, SplitString) select '%1',_id,'%2','%3' from WritingSystems where FlexString='%4';").arg(bit.id()).arg(bit.text()).arg(splitString).arg(bit.writingSystem()->flexString());
    if( !q.exec(query)  )
        qDebug() << "Project::updateInterpretationMorphologicalAnalysis" << q.lastError().text() << query;
}

QString Project::getInterpretationGloss(qlonglong id, WritingSystem *ws) const
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
        qDebug() << "Project::getInterpretationGloss" << q.lastError().text() << query;
        return "";
    }
}

QString Project::getInterpretationTranscription(qlonglong id, WritingSystem *ws) const
{
    QSqlQuery q(mDb);
    QString query = QString("select Form from PhoneticForms where InterpretationId='%1' and WritingSystem in (select _id from WritingSystems where FlexString='%2');").arg(id).arg(ws->flexString());
    if( q.exec(query) )
    {
        if( q.first() )
            return q.value(0).toString();
        else
            return "";
    }
    else
    {
        qDebug() << "Project::getInterpretationTranscription" << q.lastError().text() << query;
        return "";
    }
}

QString Project::getInterpretationOrthography(qlonglong id, WritingSystem *ws) const
{
    QSqlQuery q(mDb);
    QString query = QString("select Form from OrthographicForms where InterpretationId='%1' and WritingSystem in (select _id from WritingSystems where FlexString='%2');").arg(id).arg(ws->flexString());
    if( q.exec(query) )
    {
        if( q.first() )
            return q.value(0).toString();
        else
            return "";
    }
    else
    {
        qDebug() << "Project::getInterpretationOrthography" << q.lastError().text() << query;
        return "";
    }
}

QString Project::getInterpretationMorphologicalAnalysis(qlonglong id, WritingSystem *ws) const
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
        qDebug() << "Project::getInterpretationMorphologicalAnalysis" << q.lastError().text() << query;
        return "";
    }
}


