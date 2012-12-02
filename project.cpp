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

    if(maybeDelete(tempDir))
    {
        mDatabasePath = tempDir.absoluteFilePath(mDatabaseFilename);
        mDbAdapter = new DatabaseAdapter(mDatabasePath);
        mDbAdapter->initialize(mDatabasePath);
        return true;
    }
    else
    {
        mDatabasePath = tempDir.absoluteFilePath(mDatabaseFilename);
        mDbAdapter = new DatabaseAdapter(mDatabasePath);
        return false;
    }
}

bool Project::maybeDelete(QDir tempDir)
{
    QStringList files;
    files = tempDir.entryList(QStringList("*"), QDir::Files | QDir::NoSymLinks);
    if( files.count() > 0 )
    {
        if( QMessageBox::Yes == QMessageBox::question( 0, tr("Overwrite files"), tr("The temporary directory %1 has files in it. If a project has not closed recently, these files could contain important data. Click 'Yes' to save this data from being overwritten. Otherwise click 'No' and the files will be deleted.").arg(tempDir.absolutePath()), QMessageBox::Yes | QMessageBox::No , QMessageBox::Yes ) )
        {
            return false;
        }
        else
        {
            for(int i=0; i<files.count(); i++)
            {
                QFile::remove(files.at(i));
            }
            return true;
        }
    }
    else
    {
        return true;
    }
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

QList<GlossLine> Project::glossLines() const
{
    QList<GlossLine> lines;
    QSqlQuery q(QSqlDatabase::database( "qt_sql_default_connection" ));

    QString query = QString("select Type, _id, Name, Abbreviation, FlexString, KeyboardCommand, Direction, FontFamily, FontSize from GlossLines,WritingSystems where GlossLines.WritingSystem=WritingSystems._id order by DisplayOrder asc;");
    if( !q.exec(query)  )
        qDebug() << "Project::glossLines" << q.lastError().text() << query;
    while( q.next() )
    {
        GlossLine::LineType type;
        QString sType = q.value(0).toString();
        if ( sType == "Text" )
            type = GlossLine::Text;
        else
            type = GlossLine::Gloss;
        lines << GlossLine(type, new WritingSystem( q.value(1).toLongLong(), q.value(2).toString(), q.value(3).toString(), q.value(4).toString(), q.value(5).toString(), (Qt::LayoutDirection)q.value(6).toInt() , q.value(7).toString() , q.value(8).toInt() ) );
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

Text* Project::newBlankText(const QString & name, WritingSystem *ws)
{
    Text *text = new Text(name,ws,this);
    mTexts.append(text);
    return text;
}

Text* Project::textFromFlexText(QFile *file, WritingSystem *ws)
{
    Text *text = new Text(file,ws,this);
    mTexts.append(text);
    return text;
}
