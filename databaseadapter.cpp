#include "databaseadapter.h"

#include <QMessageBox>
#include <QHashIterator>
#include <QXmlResultItems>
#include <QDomDocument>
#include <QXmlQuery>

#include "textbit.h"
#include "writingsystem.h"
#include "messagehandler.h"
#include "allomorph.h"
#include "morphologicalanalysis.h"

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
            TextBit bit = textIter.value();
            WritingSystem key = textIter.key();
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
    // what is the way to deal with that? treat the singleton case differently?
    // should those "orphan" text forms be permitted?
    // should I specify a baseline text form that has to be there?

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

QList<WritingSystem> DatabaseAdapter::lexicalEntryCitationForms() const
{
    return mLexicalEntryCitationForms;
}

QList<WritingSystem> DatabaseAdapter::lexicalEntryGlosses() const
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

QHash<qlonglong,QString> DatabaseAdapter::getLexicalEntryCandidates( const TextBit & bit )
{
    QHash<qlonglong,QString> candidates;

    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select LexicalEntryId from Allomorph where WritingSystem=:WritingSystem and Form=:Form;");
    q.bindValue(":WritingSystem", bit.writingSystem().id());
    q.bindValue(":Form", bit.text());
    if( q.exec() )
    {
        while( q.next() )
            candidates.insert( q.value(0).toLongLong() , tr("Better summary than %1").arg(q.value(0).toLongLong())  );
    }
    return candidates;
}

qlonglong DatabaseAdapter::addLexicalEntry( const QString & grammaticalInfo, const QList<TextBit> & glosses, const QList<TextBit> & citationForms )
{
    QSqlDatabase db = QSqlDatabase::database(mFilename);
    db.transaction();

    QSqlQuery q(db);
    qlonglong id;

    try
    {
        q.prepare("insert into LexicalEntry (GrammaticalInformation) values (:GrammaticalInformation);");
        q.bindValue(":GrammaticalInformation",grammaticalInfo);

        if(!q.exec())
            throw -1;
        id = q.lastInsertId().toLongLong();

        QListIterator<TextBit> iter(glosses);
        while (iter.hasNext())
        {
            TextBit bit = iter.next();
            q.prepare("insert into LexicalEntryGloss (LexicalEntryId,WritingSystem,Form) values (:LexicalEntryId,:WritingSystem,:Form);");
            q.bindValue(":LexicalEntryId",id);
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
            q.bindValue(":LexicalEntryId",id);
            q.bindValue(":WritingSystem",bit.writingSystem().id());
            q.bindValue(":Form",bit.text());
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
    return id;
}

qlonglong DatabaseAdapter::addAllomorph( const TextBit & bit , qlonglong lexicalEntryId )
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
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

void DatabaseAdapter::addMorphologicalAnalysis( qlonglong textFormId, const MorphologicalAnalysis & allomorphs )
{
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("insert into MorphologicalAnalysisMembers (TextFormId,AllomorphId,AllomorphOrder) values (:TextFormId,:AllomorphId,:AllomorphOrder);");
    for(int i=0; i<allomorphs.count(); i++)
    {
        q.bindValue(":TextFormId", textFormId );
        q.bindValue(":AllomorphId", allomorphs.at(i).id() );
        q.bindValue(":AllomorphOrder", i );
        if( !q.exec() )
            qWarning() << "DatabaseAdapter::addMorphologicalAnalysis" << q.lastError().text() << q.executedQuery();
    }
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

TextBitHash DatabaseAdapter::lexicalItemGlosses(qlonglong id) const
{
    TextBitHash glosses;
    QSqlQuery q(QSqlDatabase::database(mFilename));
    q.prepare("select Form,WritingSystem from LexicalEntryGloss where LexicalEntryId=:LexicalEntryId;");
    q.bindValue(":LexicalEntryId",id);
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
