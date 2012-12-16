#include "databaseadapter.h"

#include <QMessageBox>
#include <QHashIterator>
using namespace std;

#include "textbit.h"
#include "writingsystem.h"

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
}

DatabaseAdapter::~DatabaseAdapter()
{
    close();
}

void DatabaseAdapter::createTables()
{
    QSqlQuery q(QSqlDatabase::database(mFilename));

    if( !q.exec("create table if not exists Interpretations ( _id integer primary key autoincrement );") )
        qWarning() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists TextForms ( _id integer primary key autoincrement, InterpretationId integer, WritingSystem integer, Form text );") )
        qWarning() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists MorphologicalAnalyses ( _id integer primary key autoincrement, InterpretationId integer, WritingSystem integer, Form text, SplitString text );") )
        qWarning() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists Glosses ( _id integer primary key autoincrement, InterpretationId integer, WritingSystem integer, Form text );") )
        qWarning() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists Allomorph ( _id integer primary key autoincrement, LexicalEntryId integer, WritingSystem integer, Form text );") )
        qWarning() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists LexicalEntry ( _id integer primary key autoincrement, GrammaticalInformation text );") )
        qWarning() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists LexicalEntryGloss ( _id integer primary key autoincrement, LexicalEntryId integer, WritingSystem integer, Form text );") )
        qWarning() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists LexicalEntryCitationForm ( _id integer primary key autoincrement, LexicalEntryId integer, WritingSystem integer, Form text );") )
        qWarning() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists WritingSystems ( _id integer primary key autoincrement, Name text, Abbreviation text, FlexString text, KeyboardCommand text, Direction integer, FontFamily text, FontSize text );") )
        qWarning() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists InterlinearTextLine ( Type text, DisplayOrder integer, WritingSystem integer );") )
        qWarning() << q.lastError().text() << q.lastQuery();

    if( !q.exec("create table if not exists PhrasalGlossLine ( DisplayOrder integer, WritingSystem integer );") )
        qWarning() << q.lastError().text() << q.lastQuery();

    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('Persian','Prs', 'prd-Arab', 'persian.ahk' , '1' , 'Scheherazade' , '16' );");
    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('Wakhi','Wak', 'wbl-Arab-AF', 'wakhi.ahk' , '1' , 'Scheherazade' , '16' );");
    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('English','Eng', 'en', 'english.ahk' , '0' , 'Times New Roman' , '12' );");
    q.exec("insert into WritingSystems ( Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize )  values ('IPA','IPA', 'wbl-Qaaa-AF-fonipa-x-Zipa', 'ipa.ahk' , '0' , 'Times New Roman' , '12' );");

    q.exec("insert into InterlinearTextLine ( Type, DisplayOrder, WritingSystem )  values ('Text','1', '4');");
    q.exec("insert into InterlinearTextLine ( Type, DisplayOrder, WritingSystem )  values ('Gloss','2', '1');");
    q.exec("insert into InterlinearTextLine ( Type, DisplayOrder, WritingSystem )  values ('Gloss','3', '3');");

    q.exec("insert into PhrasalGlossLine ( DisplayOrder, WritingSystem )  values ('1', '1');");
    q.exec("insert into PhrasalGlossLine ( DisplayOrder, WritingSystem )  values ('2', '3');");
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

qlonglong DatabaseAdapter::newInterpretation( const TextBit & bit )
{
    QSqlDatabase db = QSqlDatabase::database(mFilename);
    db.transaction();

    QSqlQuery q(db);
    QString query;
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
        if( !q.exec(query)  )
            qWarning() << "DatabaseAdapter::updateInterpretationTextForm" << q.lastError().text() << query;
    }
    catch(std::exception &e)
    {
        qWarning() << "DatabaseAdapter::newInterpretationFromOrthography" << q.lastError().text() << query;
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
    QString query;
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
        qWarning() << "DatabaseAdapter::newInterpretationFromOrthography" << q.lastError().text() << query;
        db.rollback();
        return -1;
    }
    db.commit();
    return id;
}

void DatabaseAdapter::updateInterpretationTextForm( const TextBit & bit )
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("update TextForms set Form=:Form where _id=:_id;");
    q.bindValue(":Form",bit.text());
    q.bindValue(":_id",bit.id());
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::updateInterpretationTextForm" << q.lastError().text() << q.executedQuery();
}

void DatabaseAdapter::updateInterpretationGloss( const TextBit & bit )
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("update Glosses set Form=:Form where _id=:_id;");
    q.bindValue(":Form",bit.text());
    q.bindValue(":_id",bit.id());
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::updateInterpretationGloss" << q.lastError().text() << q.executedQuery();
}

void DatabaseAdapter::updateInterpretationMorphologicalAnalysis( const TextBit & bit, const QString & splitString )
{
    // TODO implement this eventually
}

TextBitHash DatabaseAdapter::getInterpretationGlosses(qlonglong id) const
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

TextBitHash DatabaseAdapter::getInterpretationTextForms(qlonglong id) const
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
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id, Name, Abbreviation, KeyboardCommand, Direction, FontFamily, FontSize from WritingSystems where FlexString=:FlexString;");
    q.bindValue(":FlexString", flexString);
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::writingSystem" << q.lastError().text() << q.executedQuery();
    if( q.first() )
        return WritingSystem(q.value(0).toLongLong(), q.value(1).toString(), q.value(2).toString(), flexString, q.value(3).toString(), (Qt::LayoutDirection)q.value(4).toInt() , q.value(5).toString(), q.value(6).toInt() );
    else
        return WritingSystem();
}

WritingSystem DatabaseAdapter::writingSystem(qlonglong id) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id, Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize from WritingSystems where _id=:_id;");
    q.bindValue(":_id",id);
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::writingSystem" << q.lastError().text() << q.executedQuery();
    if( q.first() )
        return WritingSystem(q.value(0).toLongLong(), q.value(1).toString(), q.value(2).toString(), q.value(3).toString(), q.value(4).toString(), (Qt::LayoutDirection)q.value(5).toInt() , q.value(6).toString(), q.value(7).toInt() );
    else
        return WritingSystem();
}

QList<WritingSystem> DatabaseAdapter::writingSystems() const
{
    QList<WritingSystem> list;
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id, Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize from WritingSystems;");
    if( !q.exec()  )
        qWarning() << "DatabaseAdapter::writingSystems" << q.lastError().text() << q.executedQuery();
    while( q.next() )
        list << WritingSystem(q.value(0).toLongLong(), q.value(1).toString(), q.value(2).toString(), q.value(3).toString(), q.value(4).toString(), (Qt::LayoutDirection)q.value(5).toInt() , q.value(6).toString(), q.value(7).toInt() );
    return list;
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

QList<InterlinearItemType> DatabaseAdapter::interlinearTextLines() const
{
    QList<InterlinearItemType> lines;
    QSqlQuery q(QSqlDatabase::database(mFilename));

    QString query = QString("select Type, _id, Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize from InterlinearTextLine,WritingSystems where InterlinearTextLine.WritingSystem=WritingSystems._id order by DisplayOrder asc;");
    if( !q.exec(query)  )
        qWarning() << "DatabaseAdapter::InterlinearTextLine" << q.lastError().text() << query;
    while( q.next() )
    {
        InterlinearItemType::LineType type;
        QString sType = q.value(0).toString();
        if ( sType == "Text" )
            type = InterlinearItemType::Text;
        else
            type = InterlinearItemType::Gloss;
        lines << InterlinearItemType(type, WritingSystem( q.value(1).toLongLong(), q.value(2).toString(), q.value(3).toString(), q.value(4).toString(), q.value(5).toString(), (Qt::LayoutDirection)q.value(6).toInt() , q.value(7).toString() , q.value(8).toInt() ) );
    }
    return lines;
}

QList<InterlinearItemType> DatabaseAdapter::phrasalGlossLines() const
{
    QList<InterlinearItemType> lines;
    QSqlQuery q(QSqlDatabase::database(mFilename));

    QString query = QString("select _id, Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize from PhrasalGlossLine,WritingSystems where PhrasalGlossLine.WritingSystem=WritingSystems._id order by DisplayOrder asc;");
    if( !q.exec(query)  )
        qWarning() << "DatabaseAdapter::phrasalGlossLines" << q.lastError().text() << query;
    while( q.next() )
        lines << InterlinearItemType(InterlinearItemType::Gloss, WritingSystem( q.value(0).toLongLong(), q.value(1).toString(), q.value(2).toString(), q.value(3).toString(), q.value(4).toString(), (Qt::LayoutDirection)q.value(5).toInt() , q.value(6).toString() , q.value(7).toInt() ) );
    return lines;
}

TextBit DatabaseAdapter::glossFromId(qlonglong id) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id,Form,WritingSystem from Glosses where _id=:_id;");
    q.bindValue(":_id", id);
    if( q.exec() && q.next() )
    {
        return TextBit( q.value(1).toString() , writingSystem( q.value(2).toLongLong() ) , q.value(0).toLongLong() );
    }
    else
    {
        qWarning() << "DatabaseAdapter::glossFromId" << q.lastError().text() << q.executedQuery();
        return TextBit();
    }
}

TextBit DatabaseAdapter::textFormFromId(qlonglong id) const
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select _id,Form,WritingSystem from TextForms where _id=:_id;");
    q.bindValue(":_id", id);
    if( q.exec() && q.next() )
    {
        WritingSystem ws = writingSystem( q.value(2).toLongLong() );
        return TextBit( q.value(1).toString() , ws , q.value(0).toLongLong() );
    }
    else
    {
        qWarning() << "DatabaseAdapter::textFormFromId" << q.lastError().text() << q.executedQuery();
        return TextBit();
    }
}

QString DatabaseAdapter::dbFilename() const
{
    return mFilename;
}

