#include "project.h"
#include "interlinearitemtype.h"
#include "textbit.h"
#include "databaseadapter.h"
#include "sound.h"

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include <QMessageBox>
#include <QtDebug>
#include <QCryptographicHash>
#include <QXmlResultItems>
#include <QDomDocument>
#include <QXmlQuery>
#include <QDesktopServices>
#include <QProgressDialog>
#include <QAction>

#include "messagehandler.h"
#include "xsltproc.h"
#include "mainwindow.h"
#include "flextextimporter.h"

Project::Project(MainWindow * mainWindow)
{
    mMainWindow = mainWindow;
    mDatabaseFilename = "sqlite3-database.db";
    mDbAdapter = 0;
}

Project::~Project()
{
    if( mDbAdapter != 0 )
        delete mDbAdapter;
    qDeleteAll(mTexts);
    qDeleteAll(mViews);
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
            QMessageBox::critical(0,tr("Error opening file"),tr("Something seems to be wrong with the archive. The file %1, which is an important one, could not be found.").arg(mDatabaseFilename));
            return false;
        }
        if(!QFile::exists(tempDir.absoluteFilePath("configuration.xml")))
        {
            QMessageBox::critical(0,tr("Error opening file"),tr("Something seems to be wrong with the archive. The file configuration.xml, which is an important one, could not be found."));
            return false;
        }
        else
        {
            parseConfigurationFile();
            mDbAdapter->parseConfigurationFile( tempDir.absoluteFilePath("configuration.xml") );
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

Text* Project::newText(const QString & name, const WritingSystem & ws, const QString & content )
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
        text->saveText(false);
        mTexts.insert(name, text);
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

Text* Project::importFlexText(const QString & filePath,  const WritingSystem & ws)
{
    QFileInfo info(filePath);

    Text *text = new Text(ws,info.baseName(),this);

    FlexTextImporter importer(text);
    importer.readFile(filePath);

    if(text->isValid())
    {
        text->saveText(false);
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
        // save the file if it doesn't exist in the temp directory
        if( !QFile::exists( filepathFromName(text->name()) ) )
            text->saveText(false);
        mTexts.insert(text->name(), text);
        mTextPaths << filePath;
        return text;
    }
    else
    {
        switch( text->readResult() )
        {
        case FlexTextReader::FlexTextReadSuccess:
            QMessageBox::critical(0,tr("Error reading file"),tr("The text %1 could not be opened, but no error was reported.").arg(text->name()));
            break;
        case FlexTextReader::FlexTextReadBaselineNotFound:
            QMessageBox::critical(0,tr("Error reading file"),tr("The text %1 could not be opened because the baseline text could not be read.").arg(text->name()));
            break;
        case FlexTextReader::FlexTextReadXmlReadError:
            QMessageBox::critical(0,tr("Error reading file"),tr("The text %1 could not be opened because of a low-level XML reading error. ").arg(text->name()));
            break;
        case FlexTextReader::FlexTextReadNoAttempt:
            QMessageBox::critical(0,tr("Error reading file"),tr("The text %1 could not be opened because no attempt was made to open it. (Figure that one out!)").arg(text->name()));
            break;
        }

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

Project::OpenResult Project::openText(const QString & name)
{
    // if the text is already available
    if( mTexts.contains(name) )
        return Project::Success;

    QString filePath = filepathFromName(name);
    if( QFile::exists( filePath ) )
    {
        textFromFlexText( filePath );
        return Project::Success;
    }
    else
    {
        return Project::FileNotFound;
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

void Project::saveAndCloseText(Text *text)
{
    text->saveText(false);
    mTexts.remove( text->name() );
    delete text;
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
        iter.value()->saveText(false);
    }
}

void Project::closeOpenTexts( const QStringList & except )
{
    QHashIterator<QString,Text*> iter(mTexts);
    while(iter.hasNext())
    {
        iter.next();
        if( ! except.contains( iter.value()->name() ) )
            closeText( iter.value() );
    }
    mTexts.clear();
}

QString Project::doDatabaseCleanup()
{
    QProgressDialog progress(tr("Cleaning up database..."), QString(), 0, 4, 0);
    progress.setWindowModality(Qt::WindowModal);
    progress.setValue( progress.value() + 1 );

    int nRemovedInterpretations = removeUnusedInterpretations();
    progress.setValue( progress.value() + 1 );

    int nRemovedGlosses = removeUnusedGlosses();
    progress.setValue( progress.value() + 1 );

    int nRemovedTextForms = removeUnusedTextForms();

    int analysisMembers = mDbAdapter->removeUnusedMorphologicalAnalysisMembers();
    int allomorphs = mDbAdapter->removeUnusedAllomorphs();
    int lexicalEntries = mDbAdapter->removeUnusedLexicalEntries();

    return tr("Removed: %1 unused interpretations, %2 unused glosses, %3 unused text forms, %4 morphological analysis members, %5 allomorphs, %6 lexical entries").arg(nRemovedInterpretations).arg(nRemovedGlosses).arg(nRemovedTextForms).arg(analysisMembers).arg(allomorphs).arg(lexicalEntries);
}

int Project::removeUnusedInterpretations()
{
    QSet<qlonglong> fromDatabase = mDbAdapter->interpretationIds();
    QSet<qlonglong> fromTexts = getAllInterpretationIds();
    fromDatabase.subtract( fromTexts );
    return mDbAdapter->removeInterpretations( fromDatabase );
}

int Project::removeUnusedGlosses()
{
    QSet<qlonglong> fromDatabase = mDbAdapter->glossIds();
    QSet<qlonglong> fromTexts = getAllGlossIds();
    fromDatabase.subtract( fromTexts );
    return mDbAdapter->removeGlosses( fromDatabase );
}

int Project::removeUnusedTextForms()
{
    QSet<qlonglong> fromDatabase = mDbAdapter->textFormIds();
    QSet<qlonglong> fromTexts = getAllTextFormIds();
    fromDatabase.subtract( fromTexts );
    return mDbAdapter->removeTextForms( fromDatabase );
}

QSet<qlonglong> Project::getAllInterpretationIds()
{
    QSet<qlonglong> ids;
    QSetIterator<QString> iter(mTextPaths);
    while(iter.hasNext())
        ids.unite( getSetOfNumbersFromTextQuery( iter.next(), "declare namespace abg = \"http://www.adambaker.org/gloss.php\"; declare variable $path external; for $x in doc($path)/document/interlinear-text/paragraphs/paragraph/phrases/phrase/words/word return string( $x/@abg:id )" ) );
    return ids;
}

QSet<qlonglong> Project::getAllGlossIds()
{
    QSet<qlonglong> ids;
    QSetIterator<QString> iter(mTextPaths);
    while(iter.hasNext())
        ids.unite( getSetOfNumbersFromTextQuery( iter.next(), "declare namespace abg = \"http://www.adambaker.org/gloss.php\"; declare variable $path external; for $x in doc($path)/document/interlinear-text/paragraphs/paragraph/phrases/phrase/words/word/item[@type='gls'] return string( $x/@abg:id )" ) );
    return ids;
}

QSet<qlonglong> Project::getAllTextFormIds()
{
    QSet<qlonglong> ids;
    QSetIterator<QString> iter(mTextPaths);
    while(iter.hasNext())
        ids.unite( getSetOfNumbersFromTextQuery( iter.next(), "declare namespace abg = \"http://www.adambaker.org/gloss.php\"; declare variable $path external; for $x in doc($path)/document/interlinear-text/paragraphs/paragraph/phrases/phrase/words/word/item[@type='txt'] return string( $x/@abg:id )" ) );
    return ids;
}

QSet<qlonglong> Project::getSetOfNumbersFromTextQuery(const QString & filepath, const QString & queryString)
{
    QSet<qlonglong> ids;
    QXmlResultItems result;

    QXmlQuery query(QXmlQuery::XQuery10);
    query.bindVariable("path", QVariant(QUrl(filepath).path()));
    query.setMessageHandler(new MessageHandler("Project::getSetOfNumbersFromTextQuery", this));
    query.setQuery(queryString);
    query.evaluateTo(&result);
    QXmlItem item(result.next());
    while (!item.isNull())
    {
        ids << item.toAtomicValue().toLongLong();
        item = result.next();
    }
    return ids;
}

void Project::setTextXmlFromDatabase()
{
    QStringList names = textNames();
    QStringListIterator iter(names);

    QProgressDialog progress(tr("Setting FlexText text..."), tr("Cancel"), 0, names.count(), 0);
    progress.setWindowModality(Qt::WindowModal);
    int i=0;
    while(iter.hasNext())
    {
        QString textName = iter.next();

        progress.setValue(i++);
        progress.setLabelText( tr("Setting FlexText text... %1").arg(textName) );
        if( openText(textName) == Project::Success )
        {
            mTexts[textName]->saveText(true, true);
            closeText( mTexts[textName] );
        }
        if( progress.wasCanceled() )
            break;
    }
    progress.setValue(names.count());
}

bool Project::countItemsInTexts(const QString & filename, const QString & typeString)
{
    QList<qlonglong> ids;

    QFile out(filename);
    if( !out.open(QFile::WriteOnly) )
        return false;
    QTextStream stream(&out);
    stream.setCodec("UTF-8");
    stream << "'Id','Count','Form','WritingSystem'\n";

    QSetIterator<QString> iter(mTextPaths);
    while(iter.hasNext())
        ids.append( getListOfNumbersFromXQuery( iter.next(), QString("declare namespace abg = \"http://www.adambaker.org/gloss.php\"; declare variable $path external; for $x in doc($path)/document/interlinear-text/paragraphs/paragraph/phrases/phrase/words/word/item[@type='%1'] return string( $x/@abg:id )").arg(typeString) ) );

    QSet<qlonglong> set = ids.toSet();
    QSetIterator<qlonglong> setIter(set);
    TextBit bit;
    while(setIter.hasNext())
    {
        qlonglong id;
        id = setIter.next();
        if( typeString == "txt" )
            bit = mDbAdapter->textFormFromId(id);
        else
            bit = mDbAdapter->glossFromId(id);
        stream << QString("'%1','%2','%3','%4'\n").arg(id).arg(ids.count(id)).arg(bit.text()).arg(bit.writingSystem().name());
    }
    out.close();

    return true;
}

QList<qlonglong> Project::getListOfNumbersFromXQuery(const QString & filepath, const QString & queryString)
{
    QList<qlonglong> list;
    QXmlResultItems result;

    QXmlQuery query(QXmlQuery::XQuery10);
    query.bindVariable("path", QVariant(QUrl(filepath).path()));
    query.setMessageHandler(new MessageHandler("Project::getListOfNumbersFromXQuery"));
    query.setQuery(queryString);
    query.evaluateTo(&result);
    QXmlItem item(result.next());
    while (!item.isNull())
    {
        list << item.toAtomicValue().toLongLong();
        item = result.next();
    }
    return list;
}

bool Project::interpretationUsageReport(const QString & filename)
{
    QStringList instances;
    QStringList settings;

    QList<InterlinearItemType> lineTypes = mDbAdapter->glossInterlinearLines();
    for( int i=0; i<lineTypes.count(); i++ )
        settings << ( lineTypes.at(i).type() == InterlinearItemType::Gloss ? QString("gls") : QString("txt") ) + "=" + lineTypes.at(i).writingSystem().flexString();

    QSetIterator<QString> iter(mTextPaths);
    while(iter.hasNext())
        instances.append( getInterpretationUsage( iter.next(), settings.join(",") ) );

    return outputInterpretationUsageReport( filename, instances );
}

bool Project::outputInterpretationUsageReport(const QString & filename, const QStringList & instances )
{
    QList<InterlinearItemType> lineTypes = mDbAdapter->glossInterlinearLines();
    QSet<QString> set = instances.toSet();

    QFile out(filename);
    if( !out.open(QFile::WriteOnly) )
        return false;
    QTextStream stream(&out);
    stream.setCodec("UTF-8");

    QString header = "'Interpretation Id',";
    for(int i=0; i<lineTypes.count(); i++)
        header.append(QString("'%1 Id','%1',").arg(lineTypes.at(i).writingSystem().name()));
    header.append("'Count'\n");
    stream << header;

    QSetIterator<QString> setIter(set);
    while(setIter.hasNext())
    {
        QString str = setIter.next();
        QStringList tokens = str.split(",");
        QString outputLine;
        outputLine.append(QString("'%1',").arg(tokens.at(0)));
        for(int i=0; i<lineTypes.count(); i++)
        {
            TextBit bit;
            qlonglong id = tokens.at(i+1).toLongLong();
            if( id > 0 )
            {
                if( lineTypes.at(i).type() == InterlinearItemType::Gloss )
                    bit = mDbAdapter->glossFromId( id );
                else
                    bit = mDbAdapter->textFormFromId( id );
                outputLine.append(QString("'%1','%2',").arg(tokens.at(i+1)).arg(  bit.text().replace("'","''") ));
            }
            else
            {
                outputLine.append("'-1','',");
            }
        }
        outputLine.append(QString("'%1'\n").arg(instances.count(str)));

        stream << outputLine;
    }

    out.close();

    return true;
}

QStringList Project::getInterpretationUsage(const QString & filepath, const QString & encodedSettings)
{
    QStringList result;
    QXmlQuery query(QXmlQuery::XQuery10);
    query.bindVariable("path", QVariant(QUrl(filepath).path()));

    QString queryString = "declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
        "declare variable $path external; "
                          "declare variable $settings external; "
                          "declare variable $settings-array := tokenize($settings,',' ); "
                          "declare function local:writing-system( $x as xs:string ) as xs:string { substring-after($x,'=') }; "
                          "declare function local:line-type( $x as xs:string ) as xs:string { substring-before($x,'=') } ;"
                          "declare function local:get-item( $x as element(word), $writing-system as xs:string, $type as xs:string ) as xs:string { if( $x/item[@lang=$writing-system and @type=$type] ) then string( $x/item[@lang=$writing-system and @type=$type]/@abg:id ) else string(-1) }; "
                          "for $word in doc($path)/document/interlinear-text/paragraphs/paragraph/phrases/phrase/words/word "
                          "let $myset := for $setting in $settings-array return local:get-item( $word , local:writing-system($setting) , local:line-type($setting) )"
                          "return string-join( (string( $word/@abg:id ), $myset) , ',')";

    query.bindVariable("settings", QXmlItem(encodedSettings) );
    query.setMessageHandler(new MessageHandler("Project::getInterpretationUsage"));
    query.setQuery(queryString);
    query.evaluateTo(&result);

    return result;
}

QList<LongLongPair> Project::getPairedNumbersFromXQuery(const QString & filepath, const QString & queryString)
{
    QList< QPair<qlonglong,qlonglong> > pairs;

    QStringList result;
    QXmlQuery query(QXmlQuery::XQuery10);
    query.bindVariable("path", QVariant(QUrl(filepath).path()));
    query.setMessageHandler(new MessageHandler("Project::getPairedNumbersFromXQuery"));
    query.setQuery(queryString);
    query.evaluateTo(&result);

    foreach( const QString string, result )
    {
        qlonglong first = string.left( string.indexOf(",") ).toLongLong();
        qlonglong second = string.mid( string.indexOf(",") + 1 ).toLongLong();
        pairs << QPair<qlonglong,qlonglong>(first, second);
    }

    return pairs;
}

QStringList Project::getStringListFromXQuery(const QString & filepath, const QString & queryString)
{
    QStringList result;
    QXmlQuery query(QXmlQuery::XQuery10);
    query.bindVariable("path", QVariant(QUrl(filepath).path()));
    query.setMessageHandler(new MessageHandler("Project::getStringListFromXQuery"));
    query.setQuery(queryString);
    query.evaluateTo(&result);
    return result;
}

void Project::applyXslTransformationToTexts(const QString & xslFile, const QHash<QString,QString> & parameters)
{
    QStringList names = textNames();
    QStringListIterator iter(names);
    while(iter.hasNext())
        applyXslTransformationToText( iter.next(), xslFile, parameters );
}

bool Project::applyXslTransformationToText(const QString & name, const QString & xslFile, const QHash<QString,QString> & parameters)
{
    QString currentPath = filepathFromName(name);
    QString tempOutputPath = filepathFromName(name + "-output");
    QString errorOutputPath = QDir::temp().absoluteFilePath(name + "error.txt");

    Xsltproc transformation;
    transformation.setStyleSheet( xslFile );
    transformation.setXmlFilename( currentPath );
    transformation.setErrorFilename( errorOutputPath );
    transformation.setOutputFilename( tempOutputPath );
    transformation.setParameters(parameters);
    Xsltproc::ReturnValue retval = transformation.execute();

    QFileInfo errorInfo(errorOutputPath);

    if( retval != Xsltproc::Success )
    {
        qWarning () << retval;
        return false;
    }

    if( errorInfo.size() > 0 )
        QDesktopServices::openUrl(QUrl(errorOutputPath, QUrl::TolerantMode));

    QFile::remove(currentPath);
    QFile::rename(tempOutputPath, currentPath);

    return true;
}

Concordance* Project::concordance()
{
    return &mConcordance;
}

const MainWindow* Project::mainWindow() const
{
    return mMainWindow;
}

void Project::playLine(const QString & textName, int lineNumber)
{
    QString textPath = filepathFromName(textName);

    QString result;
    QXmlQuery query(QXmlQuery::XQuery10);
    query.bindVariable("path", QVariant(QUrl(textPath).path()));
    query.setMessageHandler(new MessageHandler("Project::playLine"));
    query.bindVariable("line", QXmlItem(lineNumber) );
    query.setQuery("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                   "declare variable $path external; "
                   "declare variable $line external; "
                   "declare variable $audio-file := doc($path)/document/interlinear-text/@abg:audio-file; "
                   "declare variable $start := doc($path)/document/interlinear-text/paragraphs/paragraph/phrases/phrase/item[@type='segnum' and text()=$line]/../@abg:annotation-start; "
                   "declare variable $end := doc($path)/document/interlinear-text/paragraphs/paragraph/phrases/phrase/item[@type='segnum' and text()=$line]/../@abg:annotation-end; "
                   "string-join( ( $audio-file , $start, $end ) , ',') ");

    if( query.evaluateTo(&result) )
    {
        QStringList elements = result.split(",");
        QString audioPath = elements.at(0);
        int startTime = elements.at(1).toInt();
        int endTime = elements.at(2).toInt();

        Sound *sound = new Sound( QUrl::fromEncoded(audioPath.toUtf8()) );
        sound->playSegment( startTime, endTime );
    }
}

const QList<View*>* Project::views() const
{
    return &mViews;
}

void Project::parseConfigurationFile()
{
    QFile *file = new QFile( getTempDir().absoluteFilePath("configuration.xml") );
    file->open(QFile::ReadOnly);
    QXmlStreamReader stream(file);

    bool inTab = false;

    while (!stream.atEnd())
    {
        stream.readNext();
        QString name = stream.name().toString();
        if( stream.tokenType() == QXmlStreamReader::StartElement )
        {
            if( name == "view" )
            {
                mViews << new View( stream.attributes().value("name").toString() );
            }
            else if( name == "tab" )
            {
                inTab = true;
                mViews.last()->tabs()->append( Tab( stream.attributes().value("name").toString() ) );
            }
            else if( name == "interlinear-line" && inTab )
            {
                QString type = stream.attributes().value("type").toString();
                QString lang = stream.attributes().value("lang").toString();
                InterlinearItemType iit( type, mDbAdapter->writingSystem(lang) );
                mViews.last()->tabs()->last().addInterlinearLineType( iit );
            }
            else if( name == "phrasal-gloss" && inTab )
            {
                QString lang = stream.attributes().value("lang").toString();
                InterlinearItemType iit( InterlinearItemType::Gloss , mDbAdapter->writingSystem(lang) );
                mViews.last()->tabs()->last().addPhrasalGlossType( iit );
            }
        }
        else if( stream.tokenType() == QXmlStreamReader::EndElement && name == "tab" )
        {
            inTab = false;
        }
    }
    mMainWindow->refreshViews();
    file->close();
}

const View * Project::view(const View::Type type) const
{
    if( type == View::Full )
        return mCurrentInterlinearView;
    else
        return mCurrentQuickView;
}

void Project::setInterlinearView(QAction * action)
{
    int index = action->data().toInt();
    if( index >= 0 && index < mViews.count() )
        mCurrentInterlinearView = mViews.at(index);
}

void Project::setQuickView(QAction * action)
{
    int index = action->data().toInt();
    if( index >= 0 && index < mViews.count() )
        mCurrentQuickView = mViews.at(index);
}
