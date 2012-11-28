#include "project.h"
#include "glossline.h"
#include "textbit.h"
#include "databaseadapter.h"

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include <QMessageBox>
#include <QtDebug>
#include <QCryptographicHash>

Project::Project()
{
    mDatabaseFilename = "sqlite3-database.db";
}

Project::~Project()
{
}

bool Project::create(QString filename)
{
    mProjectPath = filename;
    QDir tempDir = getTempDir();
    mDbAdapter = new DatabaseAdapter(tempDir.absoluteFilePath(mDatabaseFilename));
    mDbAdapter->initialize(filename);
    return true;
}

bool Project::readFromFile(QString filename)
{
    mProjectPath = filename;
    QDir tempDir = getTempDir();

    QuaZip zip(mProjectPath);
    if( zip.open(QuaZip::mdUnzip) )
    {
        QuaZipFile file(&zip);
        char* data = (char*)malloc(4096);
        for(bool f=zip.goToFirstFile(); f; f=zip.goToNextFile())
        {
            if( file.open(QIODevice::ReadOnly) )
            {
                QFile outfile( tempDir.absoluteFilePath(file.getActualFileName()) );
                outfile.open(QIODevice::WriteOnly);
                QDataStream out(&outfile);
                while( !file.atEnd() )
                {
                    qint64 bytesRead = file.read(data, 4096);
                    out.writeRawData(data,(uint)bytesRead);
                }
                file.close();
            }
        }
        free(data);
        zip.close();

        if(QFile::exists(tempDir.absoluteFilePath(mDatabaseFilename)))
        {
            mDbAdapter = new DatabaseAdapter(tempDir.absoluteFilePath(mDatabaseFilename));
            readTextPaths();
        }
        else
        {
            QMessageBox::critical(0,tr("Error opening file"),tr("Something seems to be wrong with the database. The file %1, which is an important one, could not be found.").arg(mDatabaseFilename));
            return false;
        }
    }
    else
    {
        QMessageBox::critical(0,tr("Error opening file"),tr("The file %1 could not be opened. It may be the wrong format, or it may have been corrupted.").arg(mProjectPath));
        return false;
    }
    return true;
}

QList<GlossLine> Project::glossLines()
{
    QList<GlossLine> lines;
    //    QSqlQuery q(mDb);
    QSqlQuery q(QSqlDatabase::database( "qt_sql_default_connection" ));

    QString query = QString("select Type, Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize from GlossLines,WritingSystems where GlossLines.WritingSystem=WritingSystems._id order by DisplayOrder asc;");
    if( !q.exec(query)  )
        qDebug() << "Project::glossLines" << q.lastError().text() << query;
    while( q.next() )
    {
        GlossLine::LineType type;
        QString sType = q.value(0).toString();
        if( sType == "Orthography" )
            type = GlossLine::Orthography;
        else if ( sType == "Transcription" )
            type = GlossLine::Transcription;
        else if ( sType == "Gloss" )
            type = GlossLine::Gloss;
        else
            type = GlossLine::Orthography;
        lines << GlossLine(type, new WritingSystem( q.value(1).toString(), q.value(2).toString(), q.value(3).toString(), q.value(4).toString(), (Qt::LayoutDirection)q.value(5).toInt() , q.value(6).toString() , q.value(7).toInt() ) );
    }
    return lines;
}

DatabaseAdapter* Project::dbAdapter()
{
    return mDbAdapter;
}

QDir Project::getTempDir()
{
    QString hash = QString(QCryptographicHash::hash(mProjectPath.toUtf8(),QCryptographicHash::Md5).toHex());
    QString name = QString("gloss-%1").arg( hash );

    QDir tempDir = QDir::temp();
    tempDir.mkdir(name);
    tempDir.cd(name);

    mTempPath = tempDir.absolutePath();

    return tempDir;
}

void Project::readTextPaths()
{
    QDir tempDir = getTempDir();
    QStringList filters;
    filters << "*.flextext";
    tempDir.setNameFilters(filters);
    mTextPaths.clear();
    mTextPaths = tempDir.entryList(QDir::Files,QDir::Name);
}

Text* Project::newBlankText(const QString & name, Text::BaselineMode bm, WritingSystem *ws)
{
    Text *text = new Text(name,bm,ws,this);
    mTexts.append(text);
    return text;
}

Text* Project::textFromFlexText(QFile *file, Text::BaselineMode bm, WritingSystem *ws)
{
    Text *text = new Text(file,bm,ws,this);
    mTexts.append(text);
    return text;
}
