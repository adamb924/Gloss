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
#include <QFileDialog>

#include "messagehandler.h"
#include "xsltproc.h"
#include "mainwindow.h"
#include "flextextimporter.h"
#include "annotationtype.h"
#include "writingsystem.h"

Project::Project(MainWindow * mainWindow) :
    mMainWindow(mainWindow), mDatabaseFilename("sqlite3-database.db"), mDbAdapter(0), mOverrideMediaPath(false), mMemoryMode(Project::OneAtATime), mCurrentInterlinearView(0), mCurrentQuickView(0)
{
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

    mDatabasePath = tempDir.absoluteFilePath(mDatabaseFilename);
    mDbAdapter = new DatabaseAdapter(mDatabasePath);

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

const DatabaseAdapter *Project::dbAdapter() const
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
    return QString("gloss-%1").arg( QFileInfo(mProjectPath).fileName() );
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

Text* Project::newText(const QString & name, const WritingSystem & ws, const QString & content, const QRegularExpression & delimiter )
{
    if( mTexts.contains(name) )
    {
        QMessageBox::critical(0, tr("Cannot add text"), tr("The project already has a text with that name. You need to rename the file if you want to add it to this project."));
        return 0;
    }

    Text *text = new Text(ws,name,this);

    text->setGlossItemsFromBaseline(content, delimiter);

    if( text->isValid() )
    {
        text->saveText(false, true,true);
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

QString Project::nameFromFilepath(const QString & path) const
{
    return QFileInfo(path).baseName();
}

Text* Project::importFlexText(const QString & filePath,  const WritingSystem & ws)
{
    QFileInfo info(filePath);

    Text *text = new Text(ws,info.baseName(),this);

    FlexTextImporter importer(text);
    importer.readFile(filePath);

    if(text->isValid())
    {
        text->saveText(false,true,false);
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
            text->saveText(false,true,false);
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
    serializeConfigurationXml();

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

QHash<QString,Text*>* Project::openedTexts()
{
    return &mTexts;
}

Text* Project::text(const QString & name)
{
    if( mTexts.contains(name) )
    {
        return mTexts[name];
    }
    else
    {
        if( openText(name) == Project::Success )
        {
            return mTexts[name];
        }
        else
        {
            return 0;
        }
    }
}

Project::OpenResult Project::openText(const QString & name)
{
    return openTextFromPath( filepathFromName(name) );
}

Project::OpenResult Project::openTextFromPath(const QString & path)
{
    if( mTexts.contains( nameFromFilepath(path) ) )
        return Project::Success;
    if( QFile::exists( path ) )
    {
        textFromFlexText( path );
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
    text->saveText(false,true,false);
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
    Text * text = mTexts.value(textName);
    if( text != 0 )
    {
        delete text;
    }
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
        iter.value()->saveText(false,true,false);
    }
}

void Project::closeOpenTexts( const QStringList & except )
{
    QHashIterator<QString,Text*> iter(mTexts);
    while(iter.hasNext())
    {
        iter.next();
        if( ! except.contains( iter.value()->name() ) )
        {
            closeText( iter.value() );
        }
    }
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
    query.bindVariable("path", QVariant(QUrl::fromLocalFile(filepath).path(QUrl::FullyEncoded)));
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
            mTexts[textName]->saveText(true, true, true);
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
    query.bindVariable("path", QVariant(QUrl::fromLocalFile(filepath).path(QUrl::FullyEncoded)));
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
    Q_UNUSED(filename);
    QMessageBox::critical(0,tr("Broken feature"),tr("This feature used to work, but relied on an outmoded function. It will need to be rewritten before it works again. This message is from Project::interpretationUsageReport()."));
    return false;
/*
    QStringList instances;
    QStringList settings;

    QList<InterlinearItemType> lineTypes = mDbAdapter->glossInterlinearLines();
    for( int i=0; i<lineTypes.count(); i++ )
        settings << ( lineTypes.at(i).type() == InterlinearItemType::Gloss ? QString("gls") : QString("txt") ) + "=" + lineTypes.at(i).writingSystem().flexString();

    QSetIterator<QString> iter(mTextPaths);
    while(iter.hasNext())
        instances.append( getInterpretationUsage( iter.next(), settings.join(",") ) );

    return outputInterpretationUsageReport( filename, instances );
*/
}

bool Project::outputInterpretationUsageReport(const QString & filename, const QStringList & instances )
{
    Q_UNUSED(filename);
    Q_UNUSED(instances);
    QMessageBox::critical(0,tr("Broken feature"),tr("This feature used to work, but relied on an outmoded function. It will need to be rewritten before it works again. This message is from Project::outputInterpretationUsageReport()."));
    return false;
/*
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
    */
}

QStringList Project::getInterpretationUsage(const QString & filepath, const QString & encodedSettings)
{
    QStringList result;
    QXmlQuery query(QXmlQuery::XQuery10);
    query.bindVariable("path", QVariant(QUrl::fromLocalFile(filepath).path(QUrl::FullyEncoded)));

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
    query.bindVariable("path", QVariant(QUrl::fromLocalFile(filepath).path(QUrl::FullyEncoded)));
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
    query.bindVariable("path", QVariant(QUrl::fromLocalFile(filepath).path(QUrl::FullyEncoded)));
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

    if( ! QFileInfo(textPath).exists() )
        return;

    QString result;
    QXmlQuery query(QXmlQuery::XQuery10);
    query.bindVariable("path", QVariant(QUrl::fromLocalFile(textPath).path(QUrl::FullyEncoded)));
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
        QString audioPath = mediaPath(elements.at(0));
        int startTime = elements.at(1).toInt();
        int endTime = elements.at(2).toInt();

        if( QFileInfo::exists(audioPath) )
        {
            Sound *sound = new Sound( QUrl::fromEncoded( audioPath.toUtf8() ) );
            sound->playSegment( startTime, endTime );
        }
    }
}

const QList<View*>* Project::views() const
{
    return &mViews;
}

QList<View*> *Project::views()
{
    return &mViews;
}

QString Project::configurationXmlPath() const
{
    return getTempDir().absoluteFilePath("configuration.xml");
}

void Project::parseConfigurationFile()
{
    QFile *file = new QFile( configurationXmlPath() );
    file->open(QFile::ReadOnly);
    QXmlStreamReader stream(file);

    bool inTab = false;
    WritingSystem itemTypeWs;

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
                Tab::TabType type = stream.attributes().hasAttribute("type") ? Tab::getType(stream.attributes().value("type").toString()) : Tab::InterlinearDisplay;
                mViews.last()->tabs()->append( new Tab( stream.attributes().value("name").toString() , type ) );
            }
            else if( name == "item-type" )
            {
                inTab = true;
                if( stream.attributes().hasAttribute("baseline-writing-system") )
                {
                    itemTypeWs = mDbAdapter->writingSystem( stream.attributes().value("baseline-writing-system").toString() );
                }
                else
                {
                    itemTypeWs = WritingSystem();
                }
            }
            else if( name == "interlinear-line" && inTab )
            {
                QString type = stream.attributes().value("type").toString();
                QString lang = stream.attributes().value("lang").toString();
                InterlinearItemType iit( type, mDbAdapter->writingSystem(lang) );
                if( itemTypeWs.isValid() )
                {
                    mViews.last()->tabs()->last()->addInterlinearLineType( itemTypeWs, iit );
                }
            }
            else if( name == "phrasal-gloss" && inTab )
            {
                QString lang = stream.attributes().value("lang").toString();
                InterlinearItemType iit( InterlinearItemType::Gloss , mDbAdapter->writingSystem(lang) );
                mViews.last()->tabs()->last()->addPhrasalGlossType( iit );
            }
            else if( name == "media-folder" )
            {
                if( stream.attributes().hasAttribute("override-paths") && stream.attributes().value("override-paths").toString() == "true" )
                {
                    mOverrideMediaPath = true;
                }
                mMediaPath = QUrl(stream.readElementText()).toLocalFile();
            }
        }
        else if( stream.tokenType() == QXmlStreamReader::EndElement && name == "tab" )
        {
            inTab = false;
        }
    }

    mLexicalEntryCitationForms = writingSystemListFromConfigurationFile("declare variable $path external; for $i in doc($path)/gloss-configuration/lexical-entry-citation-forms/citation-form return string($i/@lang)");
    mLexicalEntryGlosses = writingSystemListFromConfigurationFile("declare variable $path external; for $i in doc($path)/gloss-configuration/lexical-entry-glosses/gloss return string($i/@lang)");

    languageSettingsFromConfigurationFile();
    annotationTypesFromConfigurationFile();

    maybeUpdateMediaPath();

    mMainWindow->setGuiElementsFromProject();
    file->close();
}

const View * Project::view(const View::Type type) const
{
    if( type == View::Full )
        return mCurrentInterlinearView;
    else
        return mCurrentQuickView;
}

void Project::setView(int index)
{
    if( index >= 0 && index < mViews.count() )
    {
        if( mCurrentInterlinearView != mViews.at(index) )
        {
            mCurrentInterlinearView = mViews.at(index);
            emit currentViewChanged(index);
        }
    }
}

void Project::setQuickView(int index)
{
    if( index >= 0 && index < mViews.count() )
    {
        if( mCurrentQuickView != mViews.at(index) )
        {
            mCurrentQuickView = mViews.at(index);
            emit currentQuickViewChanged(index);
        }
    }
}

void Project::setView(QAction * action)
{
    setView( action->data().toInt() );
}

void Project::setQuickView(QAction * action)
{
    setQuickView(action->data().toInt());
}

bool Project::overrideMediaPath() const
{
    return mOverrideMediaPath;
}

void Project::setOverrideMediaPath(bool value)
{
    mOverrideMediaPath = value;
}

void Project::setMetaLanguage(const WritingSystem &ws)
{
    mMetaLanguage = ws;
}

void Project::setDefaultGlossLanguage(const WritingSystem &ws)
{
    mDefaultGlossLanguage = ws;
}

void Project::setDefaultTextFormLanguage(const WritingSystem &ws)
{
    mDefaultTextFormLanguage = ws;
}

void Project::setMediaFolder(const QString &folder)
{
    mMediaPath = QDir(folder);
}

Project::MemoryMode Project::memoryMode() const
{
    return mMemoryMode;
}

void Project::setMemoryMode( Project::MemoryMode mode )
{
    mMemoryMode = mode;

    if( mMemoryMode == Project::GreedyFast )
        loadAllTextsIntoMemory();
}

void Project::loadAllTextsIntoMemory()
{
    QProgressDialog progress(tr("Opening texts..."), QString(tr("Cancel")), 0, mTextPaths.count(), 0);
    progress.setWindowModality(Qt::WindowModal);
    progress.setValue( progress.value() + 1 );

    int i=0;

    QSetIterator<QString> iter(mTextPaths);
    while( iter.hasNext() )
    {
        openTextFromPath( iter.next() );
        progress.setValue(i++);
        if (progress.wasCanceled())
            break;
    }
    progress.setValue(mTextPaths.count());
}

void Project::baselineSearchReplace(const TextBit &search , const TextBit &replace )
{
    QMutableHashIterator<QString,Text*> iter(mTexts);
    while( iter.hasNext() )
    {
        iter.next();
        iter.value()->baselineSearchReplace( search, replace );
    }
}

QString Project::mediaPath(const QString &path) const
{
    if( mOverrideMediaPath )
    {
        QFileInfo info(path);
        return mMediaPath.absoluteFilePath(info.fileName());
    }
    else
    {
        return path;
    }
}

QString Project::mediaFolder() const
{
    return mMediaPath.path();
}

void Project::maybeUpdateMediaPath()
{
    if( !mMediaPath.exists() )
    {
        if( QMessageBox::Yes == QMessageBox::question(0,tr("Update directory?"),tr("The media directory specified in the project (%1) does not exist. Would you like to choose a new directory?").arg(mMediaPath.absolutePath())) )
        {
            QFileDialog dlg;
            dlg.setFileMode(QFileDialog::Directory);
            if(dlg.exec())
            {
                mMediaPath = dlg.directory();
            }
        }
    }
}

QList<WritingSystem> * Project::lexicalEntryCitationFormFields()
{
    return &mLexicalEntryCitationForms;
}

const QList<WritingSystem> *Project::lexicalEntryCitationFormFields() const
{
    return &mLexicalEntryCitationForms;
}

QList<WritingSystem> * Project::lexicalEntryGlossFields()
{
    return &mLexicalEntryGlosses;
}

const QList<WritingSystem> *Project::lexicalEntryGlossFields() const
{
    return &mLexicalEntryGlosses;
}

QList<WritingSystem> Project::writingSystemListFromConfigurationFile(const QString & queryString) const
{
    QList<WritingSystem> items;
    QXmlResultItems result;
    QXmlQuery query(QXmlQuery::XQuery10);
    query.setMessageHandler(new MessageHandler("DatabaseAdapter::writingSystemListFromConfigurationFile"));
    query.bindVariable("path", QVariant(QUrl::fromLocalFile(configurationXmlPath()).path(QUrl::FullyEncoded)));
    query.setQuery(queryString);
    query.evaluateTo(&result);
    QXmlItem item(result.next());
    while (!item.isNull())
    {
        items << mDbAdapter->writingSystem(item.toAtomicValue().toString());
        item = result.next();
    }
    return items;
}

void Project::languageSettingsFromConfigurationFile()
{
    QString result;
    QXmlQuery query(QXmlQuery::XQuery10);
    query.setMessageHandler(new MessageHandler("DatabaseAdapter::metalanguageFromConfigurationFile"));

    query.bindVariable("path", QVariant(QUrl::fromLocalFile(configurationXmlPath()).path(QUrl::FullyEncoded)));

    query.setQuery("string(doc($path)/gloss-configuration/meta-language/@lang)");
    query.evaluateTo(&result);
    mMetaLanguage = mDbAdapter->writingSystem(result.trimmed());

    query.setQuery("string(doc($path)/gloss-configuration/default-gloss-language/@lang)");
    query.evaluateTo(&result);
    mDefaultGlossLanguage = mDbAdapter->writingSystem(result.trimmed());

    query.setQuery("string(doc($path)/gloss-configuration/default-text-form-language/@lang)");
    query.evaluateTo(&result);
    mDefaultTextFormLanguage = mDbAdapter->writingSystem(result.trimmed());

    // in case anything's gone wrong...
    if( mMetaLanguage.isNull() )
        mMetaLanguage = mDbAdapter->writingSystems().first();
    if( mDefaultGlossLanguage.isNull() )
        mDefaultGlossLanguage = mDbAdapter->writingSystems().first();
    if( mDefaultTextFormLanguage.isNull() )
        mDefaultTextFormLanguage = mDbAdapter->writingSystems().first();
}

void Project::annotationTypesFromConfigurationFile()
{
    QStringList result;
    QXmlQuery query(QXmlQuery::XQuery10);
    query.setMessageHandler(new MessageHandler("DatabaseAdapter::annotationTypesFromConfigurationFile"));
    query.bindVariable("path", QVariant(QUrl::fromLocalFile(configurationXmlPath()).path(QUrl::FullyEncoded)));
    query.setQuery("declare variable $path external; "
                   "for $x in doc($path)/gloss-configuration/annotations/annotation "
                   "return string-join( ($x/@name, $x/@mark , $x/@lang , $x/@header-lang ) , ',') ");
    query.evaluateTo(&result);

    for(int i=0; i<result.count(); i++)
    {
        QStringList split = result.at(i).split(",");
        if( split.count() == 4 ) // current format
        {
            mAnnotationTypes << AnnotationType(split.at(0),split.at(1), mDbAdapter->writingSystem(split.at(2)), mDbAdapter->writingSystem(split.at(3)) );
        }
        else if( split.count() == 3 ) // the format when a header language wasn't specified
        {
            mAnnotationTypes << AnnotationType(split.at(0),split.at(1), mDbAdapter->writingSystem(split.at(2)), mDbAdapter->writingSystem(split.at(2)) );
        }
        else
        {
            continue;
        }
    }
}

const QList<AnnotationType>* Project::annotationTypes() const
{
    return &mAnnotationTypes;
}

QList<AnnotationType>* Project::annotationTypes()
{
    return &mAnnotationTypes;
}

AnnotationType Project::annotationType(const QString & label) const
{
    for(int i=0; i<mAnnotationTypes.count(); i++)
        if( mAnnotationTypes.at(i).label() == label )
            return mAnnotationTypes.at(i);
    return AnnotationType();
}

WritingSystem Project::metaLanguage() const
{
    return mMetaLanguage;
}

WritingSystem Project::defaultGlossLanguage() const
{
    return mDefaultGlossLanguage;
}

WritingSystem Project::defaultTextFormLanguage() const
{
    return mDefaultTextFormLanguage;
}

void Project::serializeConfigurationXml()
{
    QFile *file = new QFile( configurationXmlPath() );
    file->open(QFile::WriteOnly);
    QXmlStreamWriter stream(file);
    stream.setAutoFormatting(true);

    stream.writeStartDocument();
    stream.writeStartElement("gloss-configuration");

    stream.writeEmptyElement("meta-language");
    stream.writeAttribute("lang", metaLanguage().flexString() );

    stream.writeEmptyElement("default-text-form-language");
    stream.writeAttribute("lang", defaultTextFormLanguage().flexString() );

    stream.writeEmptyElement("default-gloss-language");
    stream.writeAttribute("lang", defaultGlossLanguage().flexString() );

    stream.writeStartElement("media-folder");
    stream.writeAttribute("override-paths", mOverrideMediaPath ? "true" : "false" );
    stream.writeCharacters( QUrl::fromLocalFile( mMediaPath.absolutePath() ).toString() + "/");
    stream.writeEndElement(); // media-folder

    stream.writeStartElement("views");
    for(int i=0; i<mViews.count(); i++)
    {
        View * view = mViews.at(i);

        stream.writeStartElement("view");
        stream.writeAttribute("name",view->name());

        for(int j=0; j<view->tabs()->count(); j++)
        {
            Tab * tab = view->tabs()->at(j);

            stream.writeStartElement("tab");
            stream.writeAttribute("name", tab->name() );
            stream.writeAttribute("type", Tab::getTypeString(tab->type()) );

            for(int k=0; k < tab->interlinearLineKeys().count(); k++)
            {
                WritingSystem ws = tab->interlinearLineKeys().at(k);

                stream.writeStartElement("item-type");
                stream.writeAttribute("baseline-writing-system", ws.flexString() );

                InterlinearItemTypeList * lines =  tab->interlinearLines(ws);
                for(int l=0; l < lines->count(); l++)
                {
                    InterlinearItemType type = lines->at(l);

                    stream.writeEmptyElement("interlinear-line");
                    stream.writeAttribute("type", type.typeXmlString() );
                    stream.writeAttribute("lang",type.writingSystem().flexString());
                }

                stream.writeEndElement(); // item-type
            }

            for( int k=0; k < tab->phrasalGlossLines()->count(); k++ )
            {
                stream.writeEmptyElement("phrasal-gloss");
                stream.writeAttribute("lang", tab->phrasalGlossLines()->at(k).writingSystem().flexString() );
            }

            stream.writeEndElement(); // tab
        }

        stream.writeEndElement(); // view
    }
    stream.writeEndElement(); // views

    stream.writeStartElement("annotations");
    for(int i=0; i < mAnnotationTypes.count(); i++ )
    {
        stream.writeEmptyElement("annotation");
        stream.writeAttribute("name", mAnnotationTypes.at(i).label() );
        stream.writeAttribute("mark", mAnnotationTypes.at(i).mark() );
        stream.writeAttribute("lang", mAnnotationTypes.at(i).writingSystem().flexString() );
        stream.writeAttribute("header-lang", mAnnotationTypes.at(i).headerWritingSystem().flexString() );
    }
    stream.writeEndElement(); // annotations

    stream.writeStartElement("lexical-entry-citation-forms");
    for(int i=0; i < mLexicalEntryCitationForms.count(); i++ )
    {
        stream.writeEmptyElement("citation-form");
        stream.writeAttribute("lang", mLexicalEntryCitationForms.at(i).flexString() );
    }
    stream.writeEndElement(); // lexical-entry-citation-forms

    stream.writeStartElement("lexical-entry-glosses");
    for(int i=0; i < mLexicalEntryGlosses.count(); i++ )
    {
        stream.writeEmptyElement("gloss");
        stream.writeAttribute("lang", mLexicalEntryGlosses.at(i).flexString() );
    }
    stream.writeEndElement(); // lexical-entry-glosses

    stream.writeEndElement(); // gloss-configuration

    file->close();
}
