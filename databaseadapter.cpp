#include "databaseadapter.h"

#include <QMessageBox>
#include <QHashIterator>
#include <QXmlResultItems>
#include <QDomDocument>
#include <QXmlQuery>
#include <QPair>
#include <QProgressDialog>

#include "textbit.h"
#include "writingsystem.h"
#include "messagehandler.h"
#include "allomorph.h"
#include "morphologicalanalysis.h"
#include "text.h"
#include "project.h"

DatabaseAdapter::DatabaseAdapter(const QString & filename, QObject *parent) :
        QObject(parent)
{
    mFilename = filename;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", mFilename);
    db.setHostName("hostname");
    db.setDatabaseName(filename);
    if(!db.open())
    {
        QMessageBox::information (0,"Error Message","There was a problem in opening the database. The program said: " + db.lastError().databaseText() + " It is unlikely that you will solve this on your own. Rather you had better contact the developer." );
        return;
    }

    loadWritingSystems();
}

DatabaseAdapter::~DatabaseAdapter()
{
    close();
}

void DatabaseAdapter::createTables()
{
    QSqlQuery q(QSqlDatabase::database(mFilename));

    if( !q.exec("create table if not exists Interpretations ( _id integer primary key autoincrement );") )
        qWarning() << q.lastError().text() << q.executedQuery();

    if( !q.exec("create table if not exists TextForms ( _id integer primary key autoincrement, InterpretationId integer, WritingSystem integer, Form text );") )
        qWarning() << q.lastError().text() << q.executedQuery();

    if( !q.exec("create table if not exists Glosses ( _id integer primary key autoincrement, InterpretationId integer, WritingSystem integer, Form text );") )
        qWarning() << q.lastError().text() << q.executedQuery();

    if( !q.exec("create table if not exists Allomorph ( _id integer primary key autoincrement, LexicalEntryId integer, WritingSystem integer, Form text );") )
        qWarning() << q.lastError().text() << q.executedQuery();

    if( !q.exec("create table if not exists LexicalEntry ( _id integer primary key autoincrement, GrammaticalInformation text );") )
        qWarning() << q.lastError().text() << q.executedQuery();

    if( !q.exec("create table if not exists LexicalEntryGloss ( _id integer primary key autoincrement, LexicalEntryId integer, WritingSystem integer, Form text );") )
        qWarning() << q.lastError().text() << q.executedQuery();

    if( !q.exec("create table if not exists LexicalEntryCitationForm ( _id integer primary key autoincrement, LexicalEntryId integer, WritingSystem integer, Form text );") )
        qWarning() << q.lastError().text() << q.executedQuery();

    if( !q.exec("create table if not exists LexicalEntryTags ( LexicalEntryId integer, TagId integer );") )
        qWarning() << q.lastError().text() << q.executedQuery();

    if( !q.exec("create table if not exists GrammaticalTags ( _id integer primary key autoincrement, Tag text unique on conflict ignore );") )
        qWarning() << q.lastError().text() << q.executedQuery();

    if( !q.exec("create table if not exists MorphologicalAnalysisMembers ( _id integer primary key autoincrement, TextFormId integer, AllomorphId integer, AllomorphOrder integer );") )
        qWarning() << q.lastError().text() << q.executedQuery();

    if( !q.exec("create table if not exists WritingSystems ( _id integer primary key autoincrement, Name text, Abbreviation text, FlexString text, KeyboardCommand text, Direction integer, FontFamily text, FontSize text );") )
        qWarning() << q.lastError().text() << q.executedQuery();

    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('Persian','Prs', 'prd-Arab', 'persian.ahk' , '1' , 'Scheherazade' , '16' );");
    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('Wakhi','Wak', 'wbl-Arab-AF', 'wakhi.ahk' , '1' , 'Scheherazade' , '16' );");
    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('English','Eng', 'en', 'english.ahk' , '0' , 'Times New Roman' , '12' );");
    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('IPA','IPA', 'wbl-Qaaa-AF-fonipa-x-Zipa', 'ipa.ahk' , '0' , 'Times New Roman' , '12' );");
    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('IPA','IPA', 'isk-Qaaa-AF-fonipa-x-Zipa', 'ipa.ahk' , '0' , 'Times New Roman' , '12' );");
    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('Ishkashimi','ISK', 'isk-Arab-AF', 'ishkashimi.ahk' , '1' , 'Scheherazade' , '16' );");
}

QList<qlonglong> DatabaseAdapter::candidateInterpretations(const TextBit & bit) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select InterpretationId from TextForms where Form=:Form and WritingSystem=:WritingSystem;");
    q.bindValue(":Form", bit.text());
    q.bindValue(":WritingSystem", bit.writingSystem().id() );

    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::candidateInterpretations" << q.lastError().text() << q.executedQuery();

    QList<qlonglong> candidates;
    while( q.next() )
        candidates << q.value(0).toLongLong();

    return candidates;
}

bool DatabaseAdapter::hasMultipleCandidateInterpretations(const TextBit & bit) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select count(distinct InterpretationId) from TextForms where Form=:Form and WritingSystem=:WritingSystem;");
    q.bindValue(":Form", bit.text());
    q.bindValue(":WritingSystem", bit.writingSystem().id() );
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::hasMultipleCandidateInterpretations" << q.lastError().text() << q.executedQuery();
    if( q.next() )
        return q.value(0).toInt() > 1;
    else
        return false;
}

QHash<qlonglong,QString> DatabaseAdapter::candidateInterpretationWithSummaries(const TextBit & bit) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select InterpretationId,group_concat(Form,', ') from ( select InterpretationId, Form from TextForms where InterpretationId in (select InterpretationId from TextForms where Form=:Form and WritingSystem=:WritingSystem) union select InterpretationId, Form from Glosses where InterpretationId in (select InterpretationId from TextForms where Form=:Form and WritingSystem=:WritingSystem) ) group by InterpretationId;");
    q.bindValue(":Form", bit.text());
    q.bindValue(":WritingSystem", bit.writingSystem().id() );

    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::candidateInterpretationWithSummaries" << q.lastError().text() << q.executedQuery();

    QHash<qlonglong,QString> candidates;
    while( q.next() )
        candidates.insert( q.value(0).toLongLong() , q.value(1).toString() );
    return candidates;
}

QHash<qlonglong,QString> DatabaseAdapter::interpretationTextForms(qlonglong interpretationId, qlonglong writingSystemId) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id,Form from TextForms where InterpretationId=:InterpretationId and WritingSystem=:WritingSystem;");
    q.bindValue(":InterpretationId",interpretationId);
    q.bindValue(":WritingSystem",writingSystemId);

    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::interpretationTextForms" << q.lastError().text() << q.executedQuery();

    QHash<qlonglong,QString> candidates;
    while( q.next() )
        candidates.insert( q.value(0).toLongLong() , q.value(1).toString() );
    return candidates;
}

QHash<qlonglong,QString> DatabaseAdapter::interpretationGlosses(qlonglong interpretationId, qlonglong writingSystemId) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id,Form from Glosses where InterpretationId=:InterpretationId and WritingSystem=:WritingSystem;");
    q.bindValue(":InterpretationId",interpretationId);
    q.bindValue(":WritingSystem",writingSystemId);

    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::interpretationGlosses" << q.lastError().text() << q.executedQuery();

    QHash<qlonglong,QString> candidates;
    while( q.next() )
        candidates.insert( q.value(0).toLongLong() , q.value(1).toString() );
    return candidates;
}

qlonglong DatabaseAdapter::newTextForm(qlonglong interpretationId, qlonglong writingSystemId)
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("insert into TextForms (InterpretationId,WritingSystem) values (:InterpretationId,:WritingSystem);");
    q.bindValue(":InterpretationId",interpretationId);
    q.bindValue(":WritingSystem",writingSystemId);

    if(q.exec())
    {
        return q.lastInsertId().toLongLong();
    }
    else
    {
        qWarning() << "DatabaseAdapter::newTextForm" << q.lastError().text() << q.executedQuery();
        return -1;
    }
}

qlonglong DatabaseAdapter::newTextForm(qlonglong interpretationId, const TextBit & bit)
{
    QSqlQuery q(QSqlDatabase::database(mFilename));

    q.prepare("select _id from TextForms where InterpretationId=:InterpretationId and WritingSystem=:WritingSystem and Form=:Form;");
    q.bindValue(":InterpretationId",interpretationId);
    q.bindValue(":WritingSystem",bit.writingSystem().id());
    q.bindValue(":Form",bit.text());
    if(q.exec())
    {
        if( q.next() )
            return q.value(0).toLongLong();
    }
    else
    {
        qWarning() << "DatabaseAdapter::newTextForm" << q.lastError().text() << q.executedQuery();
        return -1;
    }

    q.prepare("insert into TextForms (InterpretationId,WritingSystem,Form) values (:InterpretationId,:WritingSystem,:Form);");
    q.bindValue(":InterpretationId",interpretationId);
    q.bindValue(":WritingSystem",bit.writingSystem().id());
    q.bindValue(":Form",bit.text());
    if(q.exec())
    {
        return q.lastInsertId().toLongLong();
    }
    else
    {
        qWarning() << "DatabaseAdapter::newTextForm" << q.lastError().text() << q.executedQuery();
        return -1;
    }
}

qlonglong DatabaseAdapter::newGloss(qlonglong interpretationId, qlonglong writingSystemId)
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("insert into Glosses (InterpretationId,WritingSystem) values (:InterpretationId,:WritingSystem);");
    q.bindValue(":InterpretationId",interpretationId);
    q.bindValue(":WritingSystem",writingSystemId);
    if(q.exec() )
    {
        return q.lastInsertId().toLongLong();
    }
    else
    {
        qWarning() << "DatabaseAdapter::newGloss" << q.lastError().text() << q.executedQuery();
        return -1;
    }
}

qlonglong DatabaseAdapter::newGloss(qlonglong interpretationId, const TextBit & bit)
{
    QSqlQuery q(QSqlDatabase::database(mFilename));

    q.prepare("select _id from Glosses where InterpretationId=:InterpretationId and WritingSystem=:WritingSystem and Form=:Form;");
    q.bindValue(":InterpretationId",interpretationId);
    q.bindValue(":WritingSystem",bit.writingSystem().id());
    q.bindValue(":Form",bit.text());
    if(q.exec())
    {
        if( q.next() )
            return q.value(0).toLongLong();
    }
    else
    {
        qWarning() << "DatabaseAdapter::newGloss" << q.lastError().text() << q.executedQuery();
        return -1;
    }

    q.prepare("insert into Glosses (InterpretationId,WritingSystem,Form) values (:InterpretationId,:WritingSystem,:Form);");
    q.bindValue(":InterpretationId",interpretationId);
    q.bindValue(":WritingSystem",bit.writingSystem().id());
    q.bindValue(":Form",bit.text());
    if(q.exec())
    {
        return q.lastInsertId().toLongLong();
    }
    else
    {
        qWarning() << "DatabaseAdapter::newGloss" << q.lastError().text() << q.executedQuery();
        return -1;
    }
}

qlonglong DatabaseAdapter::newInterpretation( const TextBit & bit )
{
    QSqlDatabase db = QSqlDatabase::database(mFilename);
    db.transaction();

    QSqlQuery q(db);
    qlonglong id;

    try
    {
        if(!q.exec("insert into Interpretations (_id) select null;"))
            throw;
        id = q.lastInsertId().toLongLong();

        q.prepare("insert into TextForms (InterpretationId,WritingSystem,Form) values (:InterpretationId,:WritingSystem,:Form);");
        q.bindValue(":InterpretationId",id);
        q.bindValue(":WritingSystem",bit.writingSystem().id());
        q.bindValue(":Form",bit.text());
        if( !q.exec()  )
            qWarning() << "DatabaseAdapter::newInterpretation" << q.lastError().text() << q.executedQuery();
    }
    catch(std::exception &e)
    {
        qWarning() << "DatabaseAdapter::newInterpretation" << q.lastError().text() << q.executedQuery();
        db.rollback();
        return -1;
    }
    db.commit();
    return id;
}

qlonglong DatabaseAdapter::newInterpretation( TextBitHash & textForms , TextBitHash & glossForms )
{
    QSqlDatabase db = QSqlDatabase::database(mFilename);
    db.transaction();

    QSqlQuery q(QSqlDatabase::database(mFilename));
    qlonglong id;

    try
    {
        if(!q.exec("insert into Interpretations (_id) select null;"))
            throw -1;
        id = q.lastInsertId().toLongLong();

        TextBitMutableHashIterator textIter(textForms);
        while (textIter.hasNext())
        {
            textIter.next();
            q.prepare("insert into TextForms (InterpretationId,WritingSystem,Form) values (:InterpretationId,:WritingSystem,:Form);");
            q.bindValue(":InterpretationId",id);
            q.bindValue(":WritingSystem",textIter.key().id());
            q.bindValue(":Form",textIter.value().text());
            if( ! q.exec() )
                throw -1;
            textIter.value().setId( q.lastInsertId().toLongLong() );
        }

        textIter = TextBitMutableHashIterator(glossForms);
        while (textIter.hasNext())
        {
            textIter.next();
            q.prepare("insert into Glosses (InterpretationId,WritingSystem,Form) values (:InterpretationId,:WritingSystem,:Form);");
            q.bindValue(":InterpretationId",id);
            q.bindValue(":WritingSystem",textIter.key().id());
            q.bindValue(":Form",textIter.value().text());
            if( ! q.exec() )
                throw -1;
            textIter.value().setId( q.lastInsertId().toLongLong() );
        }
    }
    catch(int e)
    {
        qWarning() << "DatabaseAdapter::newInterpretation" << q.lastError().text() << q.executedQuery();
        db.rollback();
        return -1;
    }
    db.commit();
    return id;
}

void DatabaseAdapter::updateTextForm( const TextBit & bit ) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("update TextForms set Form=:Form where _id=:_id;");
    q.bindValue(":Form",bit.text());
    q.bindValue(":_id",bit.id());
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::updateInterpretationTextForm" << q.lastError().text() << q.executedQuery();
}

void DatabaseAdapter::updateGloss( const TextBit & bit ) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("update Glosses set Form=:Form where _id=:_id;");
    q.bindValue(":Form",bit.text());
    q.bindValue(":_id",bit.id());
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::updateInterpretationGloss" << q.lastError().text() << q.executedQuery();
}

void DatabaseAdapter::updateLexicalEntryCitationForm( const TextBit & bit ) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("update LexicalEntryCitationForm set Form=:Form where _id=:_id;");
    q.bindValue(":Form",bit.text());
    q.bindValue(":_id",bit.id());
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::updateLexicalEntryCitationForm" << q.lastError().text() << q.executedQuery();
}

void DatabaseAdapter::updateLexicalEntryGloss( const TextBit & bit ) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("update LexicalEntryGloss set Form=:Form where _id=:_id;");
    q.bindValue(":Form",bit.text());
    q.bindValue(":_id",bit.id());
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::updateLexicalEntryGloss" << q.lastError().text() << q.executedQuery();
}


TextBitHash DatabaseAdapter::guessInterpretationGlosses(qlonglong id) const
{
    TextBitHash glosses;
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select Form,FlexString,Glosses._id from Glosses,WritingSystems where InterpretationId=:InterpretationId and WritingSystem=WritingSystems._id;");
    q.bindValue(":InterpretationId",id);
    if( q.exec() )
        while( q.next() )
            glosses.insert( writingSystem( q.value(1).toString() ) , TextBit( q.value(0).toString() , writingSystem( q.value(1).toString() ) , q.value(2).toLongLong() ) );
    return glosses;
}

TextBitHash DatabaseAdapter::guessInterpretationTextForms(qlonglong id) const
{
    TextBitHash textForms;
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select Form,FlexString,TextForms._id from TextForms,WritingSystems where InterpretationId=:InterpretationId and WritingSystem=WritingSystems._id;");
    q.bindValue(":InterpretationId",id);
    if( q.exec() )
        while( q.next() )
            textForms.insert( writingSystem( q.value(1).toString() ) , TextBit( q.value(0).toString() , writingSystem( q.value(1).toString() ) , q.value(2).toLongLong() ) );
    return textForms;
}

WritingSystem DatabaseAdapter::writingSystem(const QString & flexString) const
{
    return mWritingSystemByFlexString.value(flexString);
}

WritingSystem DatabaseAdapter::writingSystem(qlonglong id) const
{
    return mWritingSystemByRowId.value(id);
}

QList<WritingSystem> DatabaseAdapter::writingSystems() const
{
    return mWritingSystems;
}

bool DatabaseAdapter::writingSystemExists(const QString & flexstring) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select FlexString from WritingSystems where FlexString=:FlexString;");
    q.bindValue(":FlexString", flexstring);
    q.exec();
    return q.first();
}

void DatabaseAdapter::addWritingSystem(const QString & flexString, const QString & fontFamily, Qt::LayoutDirection layoutDirection)
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("insert into WritingSystems ( FlexString, FontFamily, Direction )  values ( :FlexString , :FontFamily , :Direction );");
    q.bindValue(":FlexString", flexString );
    q.bindValue(":FontFamily", fontFamily );
    q.bindValue(":Direction", layoutDirection );
    q.exec();
}

QList<qlonglong> DatabaseAdapter::candidateInterpretations(const TextBitHash & textForms , const TextBitHash & glossForms )
{
    QString query;

    // TODO if there is no interpretation for these text forms, this method returns a list of interpretations that don't have any text forms in that writing system
    // e.g. Wakhi id 3950
    //
    // what is the way to deal with that? treat the singleton case differently?
    // should those "orphan" text forms be permitted?
    // should I specify a baseline text form that has to be there?
    // it's important to know how these are occuring in the first place
    // meantime, opening and closing each text where they appear solved the problem
    // ANSWER: they are created by a database cleanup

    TextBitHashIterator textIter(textForms);
    while (textIter.hasNext())
    {
        textIter.next();

        query.append( QString("select distinct(InterpretationId) from TextForms where (WritingSystem='%1' and Form='%2') or InterpretationId not in (select InterpretationId from TextForms where WritingSystem= '%1') ").arg( textIter.key().id() ).arg( sqlEscape( textIter.value().text() ) ) );
        if( textIter.hasNext() )
            query.append(" intersect ");
    }

    if( glossForms.count() > 0 )
        query.append(" intersect ");

    textIter = TextBitHashIterator(glossForms);
    while (textIter.hasNext())
    {
        textIter.next();

        query.append( QString("select distinct(InterpretationId) from Glosses where (WritingSystem='%1' and Form='%2') or InterpretationId not in (select InterpretationId from Glosses where WritingSystem= '%1') ").arg( textIter.key().id() ).arg( sqlEscape( textIter.value().text() ) ) );
        if( textIter.hasNext() )
            query.append(" intersect ");
    }

    query.append(";");

    qDebug() << query;

    QSqlQuery q(QSqlDatabase::database(mFilename));
    if( !q.exec(query)  )
        qWarning() << "DatabaseAdapter::candidateInterpretations" << q.lastError().text() << query;

    QList<qlonglong> candidates;
    while( q.next() )
        candidates << q.value(0).toLongLong();

    return candidates;
}

void DatabaseAdapter::close()
{
    QSqlDatabase::removeDatabase(mFilename);
}

TextBit DatabaseAdapter::glossFromId(qlonglong id) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id,Form,WritingSystem from Glosses where _id=:_id;");
    q.bindValue(":_id", id);
    if( !q.exec() )
    {
        qWarning() << "DatabaseAdapter::glossFromId" << id << q.lastError().text() << q.executedQuery();
        return TextBit();
    }
    if( q.next() )
        return TextBit( q.value(1).toString() , writingSystem( q.value(2).toLongLong() ) , q.value(0).toLongLong() );
    else
        return TextBit();
}

TextBit DatabaseAdapter::textFormFromId(qlonglong id) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id,Form,WritingSystem from TextForms where _id=:_id;");
    q.bindValue(":_id", id);
    if( !q.exec() )
    {
        qWarning() << "DatabaseAdapter::textFormFromId" << id << q.lastError().text() << q.executedQuery();
        return TextBit();
    }

    if( q.next() )
        return TextBit( q.value(1).toString() , writingSystem( q.value(2).toLongLong() ) , q.value(0).toLongLong() );
    else
        return TextBit();
}

qlonglong DatabaseAdapter::textFormId(const TextBit & bit, qlonglong interpretationId)
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id from TextForms where InterpretationId=:InterpretationId and Form=:Form and WritingSystem=:WritingSystem;");
    q.bindValue(":InterpretationId", interpretationId);
    q.bindValue(":Form", bit.text());
    q.bindValue(":WritingSystem", bit.writingSystem().id());
    if( q.exec() && q.next() )
        return q.value(0).toLongLong();
    else
        return -1;
}

qlonglong DatabaseAdapter::glossId(const TextBit & bit, qlonglong interpretationId)
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id from Glosses where InterpretationId=:InterpretationId and Form=:Form and WritingSystem=:WritingSystem;");
    q.bindValue(":InterpretationId", interpretationId);
    q.bindValue(":Form", bit.text());
    q.bindValue(":WritingSystem", bit.writingSystem().id());
    if( q.exec() && q.next() )
        return q.value(0).toLongLong();
    else
        return -1;
}


QString DatabaseAdapter::dbFilename() const
{
    return mFilename;
}

bool DatabaseAdapter::interpretationExists(qlonglong id) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id from Interpretations where _id=:_id;");
    q.bindValue(":_id", id);
    if( q.exec() )
        return q.next();
    else
        return false;
}

QSet<qlonglong> DatabaseAdapter::interpretationIds() const
{
    QSet<qlonglong> ids;
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id from Interpretations;");
    if( q.exec() )
        while( q.next() )
            ids << q.value(0).toLongLong();
    return ids;
}

QSet<qlonglong> DatabaseAdapter::textFormIds() const
{
    QSet<qlonglong> ids;
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id from TextForms;");
    if( q.exec() )
        while( q.next() )
            ids << q.value(0).toLongLong();
    return ids;
}

QSet<qlonglong> DatabaseAdapter::glossIds() const
{
    QSet<qlonglong> ids;
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id from Glosses;");
    if( q.exec() )
        while( q.next() )
            ids << q.value(0).toLongLong();
    return ids;
}

int DatabaseAdapter::removeInterpretations( QSet<qlonglong> ids )
{
    int count = 0;
    QSetIterator<qlonglong> iter(ids);
    QSqlQuery q1(QSqlDatabase::database(mFilename));
    q1.prepare("delete from Interpretations where _id=:_id;");
    QSqlQuery q2(QSqlDatabase::database(mFilename));
    q2.prepare("delete from TextForms where InterpretationId=:_id;");
    QSqlQuery q3(QSqlDatabase::database(mFilename));
    q3.prepare("delete from Glosses where InterpretationId=:_id;");
    while( iter.hasNext() )
    {
        qlonglong id = iter.next();
        q1.bindValue( ":_id", id );
        q2.bindValue( ":_id", id );
        q3.bindValue( ":_id", id );
        if( q1.exec() )
            count++;
        q2.exec();
        q3.exec();
    }
    return count;
}

int DatabaseAdapter::removeGlosses( QSet<qlonglong> ids )
{
    int count = 0;
    QSetIterator<qlonglong> iter(ids);
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("delete from Glosses where _id=:_id;");
    while( iter.hasNext() )
    {
        q.bindValue( ":_id", iter.next() );
        if( q.exec() )
            count++;
    }
    return count;
}

int DatabaseAdapter::removeTextForms( QSet<qlonglong> ids )
{
    int count = 0;
    QSetIterator<qlonglong> iter(ids);
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("delete from TextForms where _id=:_id;");
    while( iter.hasNext() )
    {
        q.bindValue( ":_id", iter.next() );
        if( q.exec() )
            count++;
    }
    return count;
}

QList<InterlinearItemType> DatabaseAdapter::glossInterlinearLines() const
{
    return mGlossInterlinearLines;
}

QList<InterlinearItemType> DatabaseAdapter::glossPhrasalGlossLines() const
{
    return mGlossPhrasalGlossLines;
}

QList<InterlinearItemType> DatabaseAdapter::analysisInterlinearLines() const
{
    return mAnalysisInterlinearLines;
}

QList<InterlinearItemType> DatabaseAdapter::analysisPhrasalGlossLines() const
{
    return mAnalysisPhrasalGlossLines;
}

QList<WritingSystem> DatabaseAdapter::lexicalEntryCitationFormFields() const
{
    return mLexicalEntryCitationForms;
}

QList<WritingSystem> DatabaseAdapter::lexicalEntryGlossFields() const
{
    return mLexicalEntryGlosses;
}

QList<InterlinearItemType> DatabaseAdapter::interlinearItemsFromConfigurationFile(const QString & queryString) const
{
    QList<InterlinearItemType> items;
    QXmlResultItems result;
    QXmlQuery query(QXmlQuery::XQuery10);
    if(!query.setFocus(QUrl( mConfigurationXmlPath )))
        return items;
    query.setMessageHandler(new MessageHandler());
    query.setQuery(queryString);
    query.evaluateTo(&result);
    QXmlItem item(result.next());
    while (!item.isNull())
    {
        QStringList values = item.toAtomicValue().toString().split(',');
        items << InterlinearItemType( values.at(0) , writingSystem(values.at(1)) );
        item = result.next();
    }
    return items;
}

void DatabaseAdapter::parseConfigurationFile(const QString & filename)
{
    mConfigurationXmlPath = filename;

    mGlossInterlinearLines = interlinearItemsFromConfigurationFile("for $i in /gloss-configuration/gloss-tab/interlinear-line return string-join( (string($i/@type), string($i/@lang)) , ',') ");
    mGlossPhrasalGlossLines = interlinearItemsFromConfigurationFile("for $i in /gloss-configuration/gloss-tab/phrasal-gloss return string-join( ('gloss', string($i/@lang)) , ',') ");
    mAnalysisInterlinearLines = interlinearItemsFromConfigurationFile("for $i in /gloss-configuration/analysis-tab/interlinear-line return string-join( (string($i/@type), string($i/@lang)) , ',') ");
    mAnalysisPhrasalGlossLines = interlinearItemsFromConfigurationFile("for $i in /gloss-configuration/analysis-tab/phrasal-gloss return string-join( ('gloss', string($i/@lang)) , ',') ");

    mLexicalEntryCitationForms = writingSystemListFromConfigurationFile("for $i in /gloss-configuration/lexical-entry-citation-forms/citation-form return string($i/@lang)");
    mLexicalEntryGlosses = writingSystemListFromConfigurationFile("for $i in /gloss-configuration/lexical-entry-glosses/gloss return string($i/@lang)");

    metalanguageFromConfigurationFile();
}

QList<WritingSystem> DatabaseAdapter::writingSystemListFromConfigurationFile(const QString & queryString) const
{
    QList<WritingSystem> items;
    QXmlResultItems result;
    QXmlQuery query(QXmlQuery::XQuery10);
    if(!query.setFocus(QUrl( mConfigurationXmlPath )))
        return items;
    query.setMessageHandler(new MessageHandler());
    query.setQuery(queryString);
    query.evaluateTo(&result);
    QXmlItem item(result.next());
    while (!item.isNull())
    {
        items << writingSystem(item.toAtomicValue().toString());
        item = result.next();
    }
    return items;
}

QHash<qlonglong,QString> DatabaseAdapter::getLexicalEntryCandidates( const TextBit & bit ) const
{
    QHash<qlonglong,QString> candidates;

    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select LexicalEntryId,_id from Allomorph where WritingSystem=:WritingSystem and Form=:Form;");
    q.bindValue(":WritingSystem", bit.writingSystem().id());
    q.bindValue(":Form", bit.text());
    if( q.exec() )
    {
        while( q.next() )
        {
            qlonglong lexicalEntryId = q.value(0).toLongLong();
            qlonglong allomorphId = q.value(1).toLongLong();

            QString summary;
            TextBitHashIterator iter( lexicalEntryCitationFormsForAllomorph( allomorphId ) );
            while(iter.hasNext())
            {
                iter.next();
                summary.append( iter.value().text() );
                if( iter.hasNext() )
                    summary.append(",");
            }
            summary.append(" ");

            iter = TextBitHashIterator( lexicalEntryGlossFormsForAllomorph( allomorphId ) );
            while(iter.hasNext())
            {
                iter.next();
                summary.append( iter.value().text() );
                if( iter.hasNext() )
                    summary.append(",");
            }

            candidates.insert( lexicalEntryId , summary );
        }
    }
    return candidates;
}

QHash<qlonglong,QString> DatabaseAdapter::searchLexicalEntries( const TextBit & bit ) const
{
    QHash<qlonglong,QString> candidates;

    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare( QString("select _id from LexicalEntry where _id in "
                       "( select LexicalEntryId from LexicalEntryGloss where WritingSystem=%2 and Form like '%1%' union "
                       "select LexicalEntryId from LexicalEntryGloss where WritingSystem=%2 and Form like '%%1%' and Form not like '%1%' union "
                       " select LexicalEntryId from LexicalEntryCitationForm where WritingSystem=%2 and Form like '%%1%' union "
                       " select LexicalEntryId from LexicalEntryCitationForm where WritingSystem=%2 and Form like '%%1%' and Form not like '%1%' "
                       " );").arg( bit.text() ).arg(bit.writingSystem().id()) );

    if( q.exec() )
    {
        while( q.next() )
        {
            qlonglong lexicalEntryId = q.value(0).toLongLong();
            candidates.insert( lexicalEntryId , lexicalEntrySummary(lexicalEntryId) );
        }
    }
    else
    {
        qWarning() << "DatabaseAdapter::searchLexicalEntries" << q.lastError().text() << q.executedQuery();
    }
    return candidates;
}

QString DatabaseAdapter::lexicalEntrySummary( qlonglong lexicalEntryId ) const
{
    QStringList items;

    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select Form from LexicalEntryGloss where LexicalEntryId=:LexicalEntryId;");
    q.bindValue(":LexicalEntryId", lexicalEntryId );
    if( q.exec() )
    {
        while( q.next() )
            items << q.value(0).toString();
    }
    else
    {
        qWarning() << "DatabaseAdapter::lexicalEntrySummary" << q.lastError().text() << q.executedQuery() << lexicalEntryId;
    }

    q.prepare("select Form from LexicalEntryCitationForm where LexicalEntryId=:LexicalEntryId;");
    q.bindValue(":LexicalEntryId", lexicalEntryId );
    if( q.exec() )
    {
        while( q.next() )
            items << q.value(0).toString();
    }
    else
    {
        qWarning() << "DatabaseAdapter::LexicalEntryCitationForm" << q.lastError().text() << q.executedQuery() << lexicalEntryId;
    }

    return items.join(", ");
}

qlonglong DatabaseAdapter::addLexicalEntry( const QString & grammaticalInfo, Allomorph::Type type, const QList<TextBit> & glosses, const QList<TextBit> & citationForms, const QStringList & grammaticalTags ) const
{
    QSqlDatabase db = QSqlDatabase::database(mFilename);
    db.transaction();

    QSqlQuery q(db);
    qlonglong lexicalEntryId;

    try
    {
        q.prepare("insert into LexicalEntry (GrammaticalInformation) values (:GrammaticalInformation);");
        q.bindValue(":GrammaticalInformation",grammaticalInfo);

        if(!q.exec())
            throw -1;
        lexicalEntryId = q.lastInsertId().toLongLong();

        QListIterator<TextBit> iter(glosses);
        while (iter.hasNext())
        {
            TextBit bit = iter.next();
            q.prepare("insert into LexicalEntryGloss (LexicalEntryId,WritingSystem,Form) values (:LexicalEntryId,:WritingSystem,:Form);");
            q.bindValue(":LexicalEntryId",lexicalEntryId);
            q.bindValue(":WritingSystem",bit.writingSystem().id());
            q.bindValue(":Form",bit.text());
            if( ! q.exec() )
                throw -1;
        }

        iter = QListIterator<TextBit>(citationForms);
        while (iter.hasNext())
        {
            TextBit bit = iter.next();
            q.prepare("insert into LexicalEntryCitationForm (LexicalEntryId,WritingSystem,Form) values (:LexicalEntryId,:WritingSystem,:Form);");
            q.bindValue(":LexicalEntryId",lexicalEntryId);
            q.bindValue(":WritingSystem",bit.writingSystem().id());
            q.bindValue(":Form",bit.text());
            if( ! q.exec() )
                throw -1;
        }

        QStringListIterator tagIter(grammaticalTags);
        while( tagIter.hasNext() )
        {
            QString tag = tagIter.next();

            q.prepare("insert or ignore into GrammaticalTags (Tag) values (:Tag);");
            q.bindValue(":Tag",tag);
            if( ! q.exec() )
                throw -1;

            q.prepare("insert or ignore into LexicalEntryTags (LexicalEntryId,TagId) values select :LexicalEntryId,_id from GrammaticalTags where Tag=:Tag;");
            q.bindValue(":LexicalEntryId",lexicalEntryId);
            q.bindValue(":Tag",tag);

            if( ! q.exec() )
                throw -1;
        }
    }
    catch(int e)
    {
        qWarning() << "DatabaseAdapter::addLexicalEntry" << q.lastError().text() << q.executedQuery();
        db.rollback();
        return -1;
    }
    db.commit();
    return lexicalEntryId;
}

qlonglong DatabaseAdapter::addAllomorph( const TextBit & bit , qlonglong lexicalEntryId ) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));

    // first check to see if this allomorph exists
    q.prepare("select _id from Allomorph where LexicalEntryId=:LexicalEntryId and WritingSystem=:WritingSystem and Form=:Form;");
    q.bindValue(":LexicalEntryId",lexicalEntryId);
    q.bindValue(":WritingSystem",bit.writingSystem().id());
    q.bindValue(":Form",bit.text());
    q.exec();
    if( q.next() )
        return q.value(0).toLongLong();

    // otherwise insert it
    q.prepare("insert into Allomorph (LexicalEntryId,WritingSystem,Form) values (:LexicalEntryId,:WritingSystem,:Form);");
    q.bindValue(":LexicalEntryId",lexicalEntryId);
    q.bindValue(":WritingSystem",bit.writingSystem().id());
    q.bindValue(":Form",bit.text());
    if( q.exec() )
    {
        return q.lastInsertId().toLongLong();
    }
    else
    {
        qWarning() << "DatabaseAdapter::addAllomorph" << q.lastError().text() << q.executedQuery();
        return -1;
    }
}

void DatabaseAdapter::setMorphologicalAnalysis( qlonglong textFormId, const MorphologicalAnalysis & morphologicalAnalysis ) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("delete from MorphologicalAnalysisMembers where TextFormId=:TextFormId;");
    q.bindValue(":TextFormId", textFormId );
    q.exec();

    q.prepare("insert into MorphologicalAnalysisMembers (TextFormId,AllomorphId,AllomorphOrder) values (:TextFormId,:AllomorphId,:AllomorphOrder);");

    AllomorphIterator iter = morphologicalAnalysis.allomorphIterator();
    int position = 0;
    while( iter.hasNext() )
    {
        q.bindValue(":TextFormId", textFormId );
        q.bindValue(":AllomorphId", iter.next().id() );
        q.bindValue(":AllomorphOrder", position );
        position++;
        if( !q.exec() )
            qWarning() << "DatabaseAdapter::setMorphologicalAnalysis" << q.lastError().text() << q.executedQuery();
    }
}

MorphologicalAnalysis DatabaseAdapter::morphologicalAnalysisFromTextFormId( qlonglong textFormId ) const
{
    TextBit textForm = textFormFromId(textFormId);
    MorphologicalAnalysis analysis( textForm );

    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select Allomorphid from MorphologicalAnalysisMembers where TextFormId=:TextFormId order by AllomorphOrder;");
    q.bindValue(":TextFormId", textFormId);
    q.exec();
    while(q.next())
    {
        qlonglong allomorphId = q.value(0).toLongLong();
        analysis.addAllomorph( allomorphFromId( allomorphId ) );
    }
    return analysis;
}

bool DatabaseAdapter::textFormHasMorphologicalAnalysis( qlonglong textFormId ) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id from MorphologicalAnalysisMembers where TextFormId=:TextFormId limit 1;");
    q.bindValue(":TextFormId", textFormId);
    if( ! q.exec() )
    {
        qWarning() << "DatabaseAdapter::textFormHasMorphologicalAnalysis" << q.lastError().text() << q.executedQuery();
        return false;
    }
    return q.next();
}

Allomorph DatabaseAdapter::allomorphFromId( qlonglong allomorphId ) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));

    q.prepare("select LexicalEntryId,Form,WritingSystem from Allomorph where _id=:_id;");
    q.bindValue(":_id", allomorphId);
    if( q.exec() )
    {
        if( q.next() )
        {
            qlonglong lexicalEntryId = q.value(0).toLongLong();
            TextBit bit( q.value(1).toString(), writingSystem( q.value(2).toLongLong() ) );
            TextBitHash glosses = lexicalItemGlosses(lexicalEntryId);
            return Allomorph(allomorphId, bit, glosses );
        }
    }
    qWarning() << "DatabaseAdapter::allomorphFromId"  << allomorphId << q.lastError().text() << q.executedQuery();
    return Allomorph();
}

WritingSystem DatabaseAdapter::metaLanguage() const
{
    return mMetaLanguage;
}

void DatabaseAdapter::metalanguageFromConfigurationFile()
{
    QString result;
    QXmlQuery query(QXmlQuery::XQuery10);
    if(!query.setFocus(QUrl( mConfigurationXmlPath )))
        return;
    query.setMessageHandler(new MessageHandler());
    query.setQuery("for $i in /gloss-configuration/meta-language return string($i/@lang)");
    query.evaluateTo(&result);
    mMetaLanguage = writingSystem(result.trimmed());
}

TextBitHash DatabaseAdapter::lexicalItemGlosses(qlonglong lexicalEntryId) const
{
    TextBitHash glosses;
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select Form,WritingSystem from LexicalEntryGloss where LexicalEntryId=:LexicalEntryId;");
    q.bindValue(":LexicalEntryId",lexicalEntryId);
    if( q.exec() )
    {
        while( q.next() )
        {
            WritingSystem ws = writingSystem( q.value(1).toLongLong() );
            glosses.insert( ws , TextBit( q.value(0).toString() , ws ) );
        }
    }
    else
    {
        qWarning() << "DatabaseAdapter::lexicalItemGlosses" << q.lastError().text() << q.executedQuery();
    }
    return glosses;
}

void DatabaseAdapter::loadWritingSystems()
{
    mWritingSystems.clear();
    mWritingSystemByRowId.clear();
    mWritingSystemByFlexString.clear();

    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id, Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize from WritingSystems;");
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::loadWritingSystems" << q.lastError().text() << q.executedQuery();
    while( q.next() )
    {
        WritingSystem ws(q.value(0).toLongLong(), q.value(1).toString(), q.value(2).toString(), q.value(3).toString(), q.value(4).toString(), (Qt::LayoutDirection)q.value(5).toInt() , q.value(6).toString(), q.value(7).toInt() );
        mWritingSystems << ws;
        mWritingSystemByRowId.insert( q.value(0).toLongLong(), ws );
        mWritingSystemByFlexString.insert( q.value(3).toString(), ws );
    }
}

TextBitHash DatabaseAdapter::lexicalEntryCitationFormsForAllomorph(qlonglong allomorphId) const
{
    TextBitHash forms;
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id,WritingSystem,Form from LexicalEntryCitationForm where LexicalEntryId in (select LexicalEntryId from Allomorph where _id=:_id);");
    q.bindValue(":_id",allomorphId);
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::lexicalEntryCitationFormsForAllomorph" << q.lastError().text() << q.executedQuery() << allomorphId;
    while( q.next() )
    {
        WritingSystem ws = writingSystem( q.value(1).toLongLong() );
        forms.insert( ws, TextBit( q.value(2).toString(), ws, q.value(0).toLongLong() ) );
    }
    return forms;
}

TextBitHash DatabaseAdapter::lexicalEntryGlossFormsForAllomorph(qlonglong allomorphId) const
{
    TextBitHash forms;
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id,WritingSystem,Form from LexicalEntryGloss where LexicalEntryId in (select LexicalEntryId from Allomorph where _id=:_id);");
    q.bindValue(":_id",allomorphId);
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::lexicalEntryGlossFormsForAllomorph" << q.lastError().text() << q.executedQuery() << allomorphId;
    while( q.next() )
    {
        WritingSystem ws = writingSystem( q.value(1).toLongLong() );
        forms.insert( ws, TextBit( q.value(2).toString(), ws, q.value(0).toLongLong() ) );
    }
    return forms;
}

QList<qlonglong> DatabaseAdapter::lexicalEntryIds() const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id from LexicalEntry;");
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::lexicalEntryIds" << q.lastError().text() << q.executedQuery();

    QList<qlonglong> ids;
    while( q.next() )
        ids << q.value(0).toLongLong();
    return ids;
}

TextBitHash DatabaseAdapter::lexicalEntryCitationFormsForId(qlonglong lexicalEntryId) const
{
    TextBitHash forms;
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id,WritingSystem,Form from LexicalEntryCitationForm where LexicalEntryId=:LexicalEntryId;");
    q.bindValue(":LexicalEntryId",lexicalEntryId);
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::lexicalEntryCitationFormsForId" << q.lastError().text() << q.executedQuery();
    while( q.next() )
    {
        WritingSystem ws = writingSystem( q.value(1).toLongLong() );
        forms.insert( ws, TextBit( q.value(2).toString(), ws, q.value(0).toLongLong() ) );
    }
    return forms;
}

TextBitHash DatabaseAdapter::lexicalEntryGlossFormsForId(qlonglong lexicalEntryId) const
{
    TextBitHash forms;
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id,WritingSystem,Form from LexicalEntryGloss where LexicalEntryId=:LexicalEntryId;");
    q.bindValue(":LexicalEntryId",lexicalEntryId);
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::lexicalEntryGlossFormsForId" << q.lastError().text() << q.executedQuery();
    while( q.next() )
    {
        WritingSystem ws = writingSystem( q.value(1).toLongLong() );
        forms.insert( ws, TextBit( q.value(2).toString(), ws, q.value(0).toLongLong() ) );
    }
    return forms;
}

QStringList DatabaseAdapter::grammaticalTagsForAllomorph(qlonglong allomorphId) const
{
    QStringList tags;
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select GrammaticalTags.Tag from GrammaticalTags,LexicalEntryTags where GrammaticalTags._id=LexicalEntryTags.TagId and LexicalEntryId in (select LexicalEntryId from Allomorph where _id=:_id);");
    q.bindValue(":_id",allomorphId);
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::grammaticalTagsForAllomorph" << q.lastError().text() << q.executedQuery() << allomorphId;
    while( q.next() )
        tags << q.value(0).toString();
    return tags;
}

QStringList DatabaseAdapter::grammaticalTags(qlonglong lexicalEntryId) const
{
    QStringList tags;
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select GrammaticalTags.Tag from GrammaticalTags,LexicalEntryTags where GrammaticalTags._id=LexicalEntryTags.TagId and LexicalEntryId=:_id;");
    q.bindValue(":_id",lexicalEntryId);
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::grammaticalTags" << q.lastError().text() << q.executedQuery();
    while( q.next() )
        tags << q.value(0).toString();
    return tags;
}

bool DatabaseAdapter::textIndicesExist() const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    if( !q.exec("select count(name) from sqlite_master where type='table' and (name='TextFormIndex' or name='GlossIndex' or name='InterpretationIndex' );") )
    {
        qWarning() << q.lastError().text() << q.executedQuery();
        return false;
    }
    if( ! q.next() )
        return false;
    return q.value(0).toInt() == 3;
}

void DatabaseAdapter::createTextIndices( const QSet<QString> * filePaths ) const
{
    createTextFormIndex(filePaths);
    createGlossIndex(filePaths);
    createInterpretationIndex(filePaths);
}

void DatabaseAdapter::createIndex( const QString & tableName, const QString & queryString, const QSet<QString> * filePaths ) const
{
    QProgressDialog progress( QObject::tr("Creating index..."), QString(), 0, filePaths->count(), 0);
    progress.setWindowModality(Qt::WindowModal);
    int position = 0;

    QSqlQuery q(QSqlDatabase::database(mFilename));

    if( !q.exec( QString("drop table if exists %1;").arg(tableName) ) )
        qWarning() << q.lastError().text() << q.executedQuery();

    if( !q.exec( QString("create table if not exists %1 ( TextName text, LineNumber integer, Id integer );").arg(tableName) ) )
        qWarning() << q.lastError().text() << q.executedQuery();

    q.prepare( QString("insert into %1 (TextName,LineNumber,Id) values (:TextName,:LineNumber,:Id)").arg(tableName) );

    QSetIterator<QString> pathIter( *filePaths );
    while( pathIter.hasNext() )
    {
        progress.setValue(position++);

        QString path = pathIter.next();
        QString textName = Text::textNameFromPath( path );

        QStringList result;
        QXmlQuery query(QXmlQuery::XQuery10);
        if(!query.setFocus(QUrl(path)))
            continue;

        query.setMessageHandler(new MessageHandler());
        query.setQuery(queryString);
        query.evaluateTo(&result);

        for( int i = 0; i < result.count(); i++ )
        {
            qlonglong first = result.at(i).left( result.at(i).indexOf(",") ).toLongLong();
            qlonglong second = result.at(i).mid( result.at(i).indexOf(",") + 1 ).toLongLong();

            q.bindValue(":TextName", textName );
            q.bindValue(":LineNumber", first );
            q.bindValue(":Id", second );
            if( !q.exec() )
                qWarning() << q.lastError().text() << q.executedQuery();
        }
    }
    progress.setValue(filePaths->count());
}

void DatabaseAdapter::createTextFormIndex( const QSet<QString> * filePaths ) const
{
    QString queryString = "declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                           "for $x in /document/interlinear-text/paragraphs/paragraph/phrases/phrase/words/word/item[@type='txt']  "
                          "let $line-number := string( $x/../../../../phrase/item[@type='segnum']/text() ) "
                          "return string-join( ($line-number, $x/@abg:id) , ',') ";
    createIndex( "TextFormIndex" , queryString , filePaths );
}

void DatabaseAdapter::createGlossIndex( const QSet<QString> * filePaths ) const
{
    QString queryString = "declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                           "for $x in /document/interlinear-text/paragraphs/paragraph/phrases/phrase/words/word/item[@type='gls']  "
                          "let $line-number := string( $x/../../../../phrase/item[@type='segnum']/text() ) "
                          "return string-join( ($line-number, $x/@abg:id) , ',') ";
    createIndex( "GlossIndex" , queryString , filePaths );
}

void DatabaseAdapter::createInterpretationIndex( const QSet<QString> * filePaths ) const
{
    QString queryString = "declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                           "for $x in /document/interlinear-text/paragraphs/paragraph/phrases/phrase/words/word "
                          "let $line-number := string( $x/../../../phrase/item[@type='segnum']/text() ) "
                          "return string-join( ($line-number, $x/@abg:id) , ',') ";
    createIndex( "InterpretationIndex" , queryString , filePaths );
}

QSqlQuery DatabaseAdapter::searchIndexForTextForm( qlonglong id ) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare( "select TextName,LineNumber,count(Id) from TextFormIndex where Id=:Id group by TextName,LineNumber order by TextName asc,LineNumber asc;" );
    q.bindValue(":Id", id);
    q.exec();
    return q;
}

QSqlQuery DatabaseAdapter::searchIndexForGloss( qlonglong id ) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare( "select TextName,LineNumber,count(Id) from GlossIndex where Id=:Id group by TextName,LineNumber order by TextName asc,LineNumber asc;" );
    q.bindValue(":Id", id);
    q.exec();
    return q;
}

QSqlQuery DatabaseAdapter::searchIndexForInterpretation( qlonglong id ) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare( "select TextName,LineNumber,count(Id) from InterpretationIndex where Id=:Id group by TextName,LineNumber order by TextName asc,LineNumber asc;" );
    q.bindValue(":Id", id);
    q.exec();
    return q;
}

QSqlQuery DatabaseAdapter::searchIndexForLexicalEntry( qlonglong id ) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare( "select TextName,LineNumber,count(Id) from TextFormIndex,MorphologicalAnalysisMembers,Allomorph where LexicalEntryId=:Id and TextFormId=Id and AllomorphId=Allomorph._id group by TextName,LineNumber order by TextName asc,LineNumber asc;" );
    q.bindValue(":Id", id);
    q.exec();
    return q;
}

QSet<qlonglong> DatabaseAdapter::lexicalEntryTextFormIds( qlonglong id ) const
{
    qDebug() << "Beginning" << QDateTime::currentDateTime ().toString(Qt::ISODate);
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare( "select TextFormIndex.Id from TextFormIndex,MorphologicalAnalysisMembers,Allomorph on TextFormId=Id and AllomorphId=Allomorph._id where LexicalEntryId=:Id;" );
    q.bindValue(":Id", id);

    QSet<qlonglong> textFormIds;
    if( !q.exec()  )
    {
        qWarning() << "DatabaseAdapter::lexicalEntryTextForms" << q.lastError().text() << q.executedQuery();
        return textFormIds;
    }

    qDebug() << "Before while" << QDateTime::currentDateTime ().toString(Qt::ISODate);
    int nResults = 0;
    while( q.next() )
    {
        qDebug() << nResults++ << QDateTime::currentDateTime().toString(Qt::ISODate);
        q.value(0).toLongLong();
    }
    qDebug() << "After while";

    return textFormIds;
}

QSet<qlonglong> DatabaseAdapter::allomorphTextFormIds( qlonglong id ) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare( "select TextFormIndex.Id from TextFormIndex,MorphologicalAnalysisMembers where AllomorphId=:Id and TextFormId=Id;" );
    q.bindValue(":Id", id);

    QSet<qlonglong> textFormIds;
    if( !q.exec()  )
    {
        qWarning() << "DatabaseAdapter::allomorphTextForms" << q.lastError().text() << q.executedQuery();
        return textFormIds;
    }
    while( q.next() )
    {
        textFormIds << q.value(0).toLongLong();
    }
    return textFormIds;
}


QSqlQuery DatabaseAdapter::searchIndexForAllomorph( qlonglong id ) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare( "select TextName,LineNumber,count(Id) from TextFormIndex,MorphologicalAnalysisMembers where AllomorphId=:Id and TextFormId=Id group by TextName,LineNumber order by TextName asc,LineNumber asc;" );
    q.bindValue(":Id", id);
    q.exec();
    return q;
}

int DatabaseAdapter::removeUnusedMorphologicalAnalysisMembers() const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.exec("delete from MorphologicalAnalysisMembers where TextFormId not in (select _id from TextForms);");
    return q.numRowsAffected();
}

int DatabaseAdapter::removeUnusedAllomorphs() const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.exec("delete from Allomorph where _id not in (select AllomorphId from MorphologicalAnalysisMembers);");
    return q.numRowsAffected();
}

int DatabaseAdapter::removeUnusedLexicalEntries() const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.exec("delete from LexicalEntry where _id not in (select LexicalEntryId from Allomorph);");
    int nLexEntriesDeleted = q.numRowsAffected();
    q.exec("delete from LexicalEntryGloss where LexicalEntryId not in (select _id from LexicalEntry);");
    q.exec("delete from LexicalEntry where LexicalEntryIdCitationForm not in (select _id from LexicalEntry);");
    q.exec("delete from LexicalEntry where LexicalEntryIdGrammaticalTags not in (select _id from LexicalEntry);");
    return nLexEntriesDeleted;
}

QStringList DatabaseAdapter::availableGrammaticalTags() const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare( "select Tag from GrammaticalTags order by Tag asc;" );
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::availableGrammaticalTags" << q.lastError().text() << q.executedQuery();

    QStringList tags;
    while( q.next() )
        tags << q.value(0).toString();
    return tags;

}

void DatabaseAdapter::setTagsForLexicalEntry( qlonglong lexicalEntryId, const QStringList & tags ) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));

    q.prepare( "delete from LexicalEntryTags where LexicalEntryId=:LexicalEntryId;" );
    q.bindValue(":LexicalEntryId", lexicalEntryId );
    q.exec();

    q.prepare( "insert into LexicalEntryTags (LexicalEntryId,TagId) select :LexicalEntryId,_id from GrammaticalTags where Tag=:Tag;" );
    q.bindValue(":LexicalEntryId", lexicalEntryId );

    QStringListIterator iter(tags);
    while(iter.hasNext())
    {
        q.bindValue(":Tag", iter.next() );
        q.exec();
    }
}

void DatabaseAdapter::renameTag( const QString & oldName, const QString & newName ) const
{
    qlonglong oldTagId = -1;
    qlonglong newTagId = -1;

    // I'm taking a few extra calls because this will be infrequently invoked, and the cost to code clarity would be high

    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare( "select _id from GrammaticalTags where Tag=:Tag;" );
    q.bindValue(":Tag", oldName );
    if( !q.exec() )
        qWarning() << q.lastError().text() << q.executedQuery();
    if( q.next() )
        oldTagId = q.value(0).toLongLong();

    q.prepare( "select _id from GrammaticalTags where Tag=:Tag;" );
    q.bindValue(":Tag", newName );
    if( !q.exec() )
        qWarning() << q.lastError().text() << q.executedQuery();
    if( q.next() )
        newTagId = q.value(0).toLongLong();

    if( newTagId != -1 ) // the new tag already exists
    {
        q.prepare("update LexicalEntryTags set TagId=:NewTagId where Tag=:OldTagId;");
        q.bindValue(":NewTagId", newTagId );
        q.bindValue(":OldTagId", oldTagId );
        if( !q.exec() )
            qWarning() << q.lastError().text() << q.executedQuery();
    }
    else // then we're just renaming a tag
    {
        q.prepare("update GrammaticalTags set Tag=:Tag where _id=:OldTagId;");
        q.bindValue(":Tag", newName );
        q.bindValue(":OldTagId", oldTagId );
        if( !q.exec() )
            qWarning() << q.lastError().text() << q.executedQuery();
    }
}

void DatabaseAdapter::removeTag( const QString & tag ) const
{
    qlonglong tagId = -1;
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare( "select _id from GrammaticalTags where Tag=:Tag;" );
    q.bindValue(":Tag", tag );
    if( !q.exec() )
        qWarning() << q.lastError().text() << q.executedQuery();
    if( q.next() )
        tagId = q.value(0).toLongLong();

    if( tagId == -1 )
        return;

    q.exec("delete from LexicalEntryTags where TagId=:TagId");
    q.bindValue(":TagId", tagId );
    if( !q.exec() )
        qWarning() << q.lastError().text() << q.executedQuery();

    q.exec("delete from GrammaticalTags where _id=:TagId");
    q.bindValue(":TagId", tagId );
    if( !q.exec() )
        qWarning() << q.lastError().text() << q.executedQuery();
}

QString DatabaseAdapter::guessGloss( const QString & hint , const WritingSystem & ws )
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare( "select Form from Glosses where InterpretationId in (select InterpretationId from Glosses where Form=:Form) and WritingSystem=:WritingSystem limit 1;" );
    q.bindValue(":Form", hint );
    q.bindValue(":WritingSystem", ws.id() );
    if( !q.exec() )
        qWarning() << q.lastError().text() << q.executedQuery();
    if( q.next() )
        return q.value(0).toString();
    return QString();
}
