#include "project.h"
#include "interlinearitemtype.h"
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
    mDbAdapter = 0;
}

Project::~Project()
{
    if( mDbAdapter != 0 )
    {
        mDbAdapter->close();
        delete mDbAdapter;
    }
    qDeleteAll(mTexts);
}

bool Project::create(QString filename)
{
    mProjectPath = filename;
    QDir tempDir = getTempDir();

    if( mDbAdapter != 0 )
    {
        delete mDbAdapter;
        mDbAdapter = 0;
    }

    if(maybeDelete(tempDir))
    {
        mDatabasePath = tempDir.absoluteFilePath(mDatabaseFilename);
        mDbAdapter = new DatabaseAdapter(mDatabasePath);
        mDbAdapter->createTables();
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
            if( mDbAdapter != 0 )
            {
                delete mDbAdapter;
                mDbAdapter = 0;
            }

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

DatabaseAdapter* Project::dbAdapter()
{
    return mDbAdapter;
}

QDir Project::getTempDir() const
{
    QString name = tempDirName();
    QDir tempDir = QDir::temp();
    tempDir.mkdir(name);
    tempDir.cd(name);
    return tempDir;
}

QString Project::tempDirName() const
{
    QString hash = QString(QCryptographicHash::hash(mProjectPath.toUtf8(),QCryptographicHash::Md5).toHex());
    return QString("gloss-%1").arg( hash );
}

void Project::readTextPaths()
{
    QDir tempDir = getTempDir();
    tempDir.setNameFilters(QStringList("*.flextext"));
    tempDir.setSorting(QDir::Name);
    mTextPaths.clear();
    QStringList entries = tempDir.entryList(QDir::Files,QDir::Name);
    for(int i=0; i<entries.count(); i++)
        mTextPaths << tempDir.absoluteFilePath(entries.at(i));
}

Text* Project::newText(const QString & name, const WritingSystem & ws, const QString & content , bool openText )
{
    if( mTexts.contains(name) )
    {
        QMessageBox::critical(0, tr("Cannot add text"), tr("The project already has a text with that name. You need to rename the file if you want to add it to this project."));
        return 0;
    }

    Text *text = new Text(ws,name,this);
    text->setBaselineText(content);
    if( text->isValid() )
    {
        text->saveText();
        if( openText )
            mTexts.insert(name, text);
        else
            delete text;
        mTextPaths << filepathFromName(name);
        return text;
    }
    else
    {
        delete text;
        return 0;
    }
}

QString Project::filepathFromName(const QString & name) const
{
    return getTempDir().absoluteFilePath( QString("%1.flextext").arg(name) );
}

Text* Project::textFromFlexText(const QString & filePath,  const WritingSystem & ws)
{
    Text *text = new Text(filePath,ws,this);
    if(text->isValid())
    {
        text->saveText();
        mTexts.insert(text->name(), text);
        mTextPaths << filePath;
        return text;
    }
    else
    {
        delete text;
        return 0;
    }
}

Text* Project::textFromFlexText(const QString & filePath)
{
    Text *text = new Text(filePath,this);
    if( text->isValid() )
    {
        text->saveText();
        mTexts.insert(text->name(), text);
        mTextPaths << filePath;
        return text;
    }
    else
    {
        delete text;
        return 0;
    }
}

bool Project::save()
{
    saveOpenTexts();

    // indebted to: http://stackoverflow.com/questions/2598117/zipping-a-folder-file-using-qt
    QuaZip zip(mProjectPath);
    QuaZipFile outFile(&zip);

    if (!zip.open(QuaZip::mdCreate))
    {
        qWarning() << "zip.open()" << zip.getZipError();
        return false;
    }
    char c;
    QFile inFile;
    QDir tempDir = getTempDir();
    tempDir.setNameFilters(QStringList("*"));
    QStringList files = tempDir.entryList(QDir::Files,QDir::Name);
    for(int i=0; i<files.count(); i++)
    {
        inFile.setFileName( tempDir.absoluteFilePath(files.at(i)) );
        if( !inFile.open(QIODevice::ReadOnly))
        {
            qWarning() << inFile.errorString();
            return false;
        }
        if( !outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(files.at(i) , tempDir.absoluteFilePath(files.at(i)) )))
        {
            qWarning() << outFile.errorString();
            return false;
        }
        while (inFile.getChar(&c) && outFile.putChar(c));

        if (outFile.getZipError() != UNZ_OK)
        {
            qWarning() << outFile.getZipError();
            return false;
        }

        outFile.close();

        if (outFile.getZipError() != UNZ_OK)
        {
            qWarning() << outFile.getZipError();
            return false;
        }

        inFile.close();
    }

    return true;
}

void Project::removeTempDirectory()
{
    mDbAdapter->close();

    QDir tempDir = getTempDir();
    tempDir.setNameFilters(QStringList("*"));
    QStringList files = tempDir.entryList(QDir::Files,QDir::Name);
    for(int i=0; i<files.count(); i++)
    {
        QFile f( tempDir.absoluteFilePath( files.at(i) ) );
        if( ! f.remove() )
            qWarning() << f.errorString() << tempDir.absoluteFilePath( files.at(i) ) ;
    }

    tempDir.cdUp();
    tempDir.rmdir( tempDirName() );
}

QSet<QString>* Project::textPaths()
{
    return &mTextPaths;
}

QHash<QString,Text*>* Project::texts()
{
    return &mTexts;
}

bool Project::openText(const QString & name)
{
    // if the text is already available
    if( mTexts.contains(name) )
        return true;

    QDir tempDir = getTempDir();
    QString filename = QString("%1.flextext").arg(name);
    QString filePath = tempDir.absoluteFilePath( filename );
    if( QFile::exists( filePath ) )
    {
        textFromFlexText( filePath );
        return true;
    }
    else
    {
        return false;
    }
}

QStringList Project::textNames() const
{
    QStringList texts;
    QFileInfo info;
    foreach( QString path , mTextPaths )
    {
        info.setFile(path);
        texts << info.baseName();
    }
    texts.sort();
    return texts;
}

void Project::closeText(Text *text)
{
    mTexts.remove( text->name() );
    delete text;
}

void Project::deleteText(QString textName)
{
    mTexts.remove( textName );
    QString path = filepathFromName(textName);
    mTextPaths.remove( path );

    QFile f( path );
    if( ! f.remove() )
        qWarning() << f.errorString() << path ;
}

QString Project::projectPath() const
{
    return mProjectPath;
}

void Project::saveOpenTexts()
{
    QHashIterator<QString,Text*> iter(mTexts);
    while(iter.hasNext())
    {
        iter.next();
        iter.value()->saveText();
    }
}
