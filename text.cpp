#include "text.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QTextStream>
#include <QtDebug>
#include <QHashIterator>
#include <QXmlQuery>
#include <QProgressDialog>
#include <QMessageBox>
#include <QDesktopServices>

#include "writingsystem.h"
#include "project.h"
#include "databaseadapter.h"
#include "phrase.h"
#include "glossitem.h"
#include "messagehandler.h"
#include "xsltproc.h"

Text::Text()
{
    mValid = false;
}

Text::Text(const WritingSystem & ws, const QString & name, Project *project)
{
    mName = name;
    mBaselineWritingSystem = ws;
    mProject = project;
    mDbAdapter = mProject->dbAdapter();
    mValid = true;
}

Text::Text(const QString & filePath, Project *project)
{
    mName = textNameFromPath(filePath);
    mProject = project;
    mDbAdapter = mProject->dbAdapter();

    QFile file(filePath);
    mValid = readTextFromFlexText(&file,true);
}

Text::Text(const QString & filePath, const WritingSystem & ws, Project *project)
{
    mName = textNameFromPath(filePath);
    mProject = project;
    mDbAdapter = mProject->dbAdapter();
    mBaselineWritingSystem = ws;

    QFile file(filePath);
    mValid = readTextFromFlexText(&file,false);
}

Text::~Text()
{
    qDeleteAll(mPhrases);
    mPhrases.clear();
}

QString Text::name() const
{
    return mName;
}

void Text::setName(const QString & name)
{
    mName = name;
}

QString Text::comment() const
{
    return mComment;
}

void Text::setComment(const QString & comment)
{
    mComment = comment;
}


WritingSystem Text::baselineWritingSystem() const
{
    return mBaselineWritingSystem;
}

void Text::setBaselineWritingSystem(const WritingSystem & ws)
{
    mBaselineWritingSystem = ws;
}
/*
WritingSystem Text::analysisWritingSystem() const
{
    return mAnalysisWritingSystem;
}

void Text::setAnalysisWritingSystem(const WritingSystem & ws)
{
    mAnalysisWritingSystem = ws;
}
*/
QString Text::baselineText() const
{
    return mBaselineText;
}

void Text::setBaselineText(const QString & text)
{
    if( text != mBaselineText )
    {
        mBaselineText = text;
        setGlossItemsFromBaseline();
        emit baselineTextChanged(mBaselineText);
    }
}

QList<Phrase*>* Text::phrases()
{
    return &mPhrases;
}

void Text::clearGlossItems()
{
    qDeleteAll(mPhrases);
    mPhrases.clear();
}

void Text::setGlossItemsFromBaseline()
{
    QStringList lines = mBaselineText.split(QRegExp("[\\n\\r]+"),QString::SkipEmptyParts);
    if( mPhrases.count() == lines.count() )
    {
        for(int i=0; i<lines.count(); i++)
        {
            if( mPhrases.at(i)->equivalentBaselineLineText() != lines.at(i) )
            {
                setLineOfGlossItems(mPhrases.at(i), lines.at(i));
            }
        }
    }
    else
    {
        clearGlossItems();
        QProgressDialog progress(tr("Analyzing text %1...").arg(mName), "Cancel", 0, lines.count(), 0);
        progress.setWindowModality(Qt::WindowModal);
        for(int i=0; i<lines.count(); i++)
        {
            progress.setValue(i);
            mPhrases.append( new Phrase );
            setLineOfGlossItems(mPhrases.last(), lines.at(i));
            if( progress.wasCanceled() )
            {
                mValid = false;
                break;
            }
        }
        progress.setValue(lines.count());
    }
}

void Text::setLineOfGlossItems( Phrase * phrase , const QString & line )
{
    qDeleteAll(*phrase);
    phrase->clear();

    QStringList words = line.split(QRegExp("[ \\t]+"),QString::SkipEmptyParts);
    for(int i=0; i<words.count(); i++)
        phrase->append(new GlossItem(TextBit(words.at(i),mBaselineWritingSystem), mProject->dbAdapter()));

    phrase->setGuiRefreshRequest(true);
}

void Text::setBaselineFromGlossItems()
{
    mBaselineText = "";
    for(int i=0; i<mPhrases.count(); i++)
    {
        QString line;
        for(int j=0; j<mPhrases.at(i)->count(); j++)
        {
            line += mPhrases.at(i)->at(j)->baselineText().text() + " ";
        }
        mBaselineText += line.trimmed() + "\n";
    }
    mBaselineText = mBaselineText.trimmed();
}

bool Text::setBaselineWritingSystemFromFile(const QString & filePath )
{
    QXmlQuery query(QXmlQuery::XQuery10);
    query.setFocus(QUrl(filePath));
    query.setMessageHandler(new MessageHandler(this));
    query.setQuery("declare namespace abg = \"http://www.adambaker.org/gloss.php\"; for $x in /document/interlinear-text/languages/language[@abg:is-baseline='true'] return string($x/@lang)");
    if (query.isValid())
    {
        QStringList result;
        query.evaluateTo(&result);

        if( result.isEmpty() )
            return false;

        mBaselineWritingSystem = mProject->dbAdapter()->writingSystem( result.at(0) );

        return true;
    }
    else
    {
        qWarning() << "Invalid query";
        return false;
    }
}

bool Text::readTextFromFlexText(QFile *file, bool baselineInfoFromFile)
{
    if( baselineInfoFromFile)
    {
        if( !setBaselineWritingSystemFromFile(file->fileName()) )
            return false;
    }

    file->open(QFile::ReadOnly);

    QFileInfo info(file->fileName());
    mName = info.baseName();

    clearGlossItems();

    int lineNumber = -1;
    bool inWord = false;
    bool inPhrase = false;
    bool hasValidId = false;
    qlonglong id = -1;
    GlossItem::ApprovalStatus approvalStatus = GlossItem::Unapproved;
    QXmlStreamReader stream(file);
    TextBitHash textForms;
    TextBitHash glossForms;
    QString baselineText = "";

    while (!stream.atEnd())
    {
        stream.readNext();
        QString name = stream.name().toString();
        if( stream.tokenType() == QXmlStreamReader::StartElement )
        {
            if( name == "word" )
            {
                inWord = true;
                textForms.clear();
                glossForms.clear();

                if( stream.attributes().hasAttribute("http://www.adambaker.org/gloss.php","id") )
                {
                    id = stream.attributes().value("http://www.adambaker.org/gloss.php","id").toString().toLongLong();
                    if( mDbAdapter->interpretationExists(id) )
                    {
                        hasValidId = true;

                        if(stream.attributes().hasAttribute("http://www.adambaker.org/gloss.php", "approval-status") && stream.attributes().value("http://www.adambaker.org/gloss.php", "approval-status").toString() == "true" )
                            approvalStatus = GlossItem::Approved;
                    }
                    else
                    {
                        hasValidId = false;
                    }
                }
                else
                {
                    hasValidId = false;
                }
            }
            else if ( name == "phrase" )
            {
                inPhrase = true;
                mPhrases.append( new Phrase );
            }
            else if ( name == "item" )
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("type") && attr.hasAttribute("lang") )
                {
                    QString type = attr.value("type").toString();
                    WritingSystem lang = mProject->dbAdapter()->writingSystem( attr.value("lang").toString() );
                    QString text = stream.readElementText();

                    if( text.isEmpty() )
                        continue;

                    if( inWord )
                    {
                        qlonglong itemId = attr.hasAttribute("http://www.adambaker.org/gloss.php","id") ? attr.value("http://www.adambaker.org/gloss.php","id").toString().toLongLong() : -1;
                        if( type == "txt" )
                        {
                            textForms.insert( lang, TextBit( text , lang, itemId) );
                            if( lang.flexString() == mBaselineWritingSystem.flexString() )
                            {
                                baselineText = text;
                            }
                        }
                        else if( type == "gls" )
                        {
                            glossForms.insert( lang, TextBit( text , lang, itemId) );
                        }
                    }
                    else if ( inPhrase && type == "gls" )
                    {
                        mPhrases.last()->setPhrasalGloss( TextBit( text , lang ) );
                    }
                    else if( type == "segnum" )
                    {
                        lineNumber = text.toInt();
                    }
                }
            }
        }
        else if( stream.tokenType() == QXmlStreamReader::EndElement )
        {
            if(name == "word")
            {
                if( baselineText.isEmpty() )
                {
                    if( id == -1 )
                    {
                        QMessageBox::information(0, tr("Parsing error"), tr("There is a word on line %1 that does not have an entry for the baseline writing system for this text, so the word has been removed. Make a note of the line number and see if your text is incomplete on that line.").arg(lineNumber) );
                        continue;
                    }
                    else
                    {
                        QMessageBox::information(0, tr("Parsing error"), tr("There is a word on line %1 that does not have an entry for the baseline writing system for this text. Since the id was there, Gloss made a guess as to the proper baseline form, but this is not foolproof. Make a note of the line number and see if your text is incomplete on that line.").arg(lineNumber) );
                    }
                }

                mPhrases.last()->append(new GlossItem( mBaselineWritingSystem, textForms, glossForms, id, mProject->dbAdapter()));
                mPhrases.last()->last()->setApprovalStatus(approvalStatus);

                // check the database for whether there are multiple interpretations for the baseline text
                if( mDbAdapter->hasMultipleCandidateInterpretations( mPhrases.last()->last()->baselineText() ) )
                    mPhrases.last()->last()->setCandidateNumber(GlossItem::MultipleOption);
                else
                    mPhrases.last()->last()->setCandidateNumber(GlossItem::SingleOption);

                inWord = false;
                hasValidId = false;
                baselineText = "";
                id = -1;
                approvalStatus = GlossItem::Unapproved;
            }
            else if(name == "phrase")
            {
                inPhrase = false;
            }
        }
    }

    file->close();

    if (stream.hasError()) {
        qWarning() << "Text::readTextFromFlexText error with xml reading";
        return false;
    }
    setBaselineFromGlossItems();
    return true;
}

bool Text::serialize(const QString & filename) const
{
    QFile outFile(filename);
    if( !outFile.open(QFile::WriteOnly | QFile::Text) )
        return false;

    QXmlStreamWriter stream(&outFile);
    stream.setCodec("UTF-8");
    stream.setAutoFormatting(true);
    stream.writeNamespace("http://www.adambaker.org/gloss.php", "abg" );

    stream.writeStartDocument();
    stream.writeStartElement("document");
    stream.writeAttribute("version", "2");

    serializeInterlinearText(&stream);

    stream.writeEndElement(); // document
    stream.writeEndDocument();

    return true;
}

bool Text::serializeInterlinearText(QXmlStreamWriter *stream) const
{
    // TODO do something else with these references to mBaselineWritingSystem
    stream->writeStartElement("interlinear-text");

    stream->writeAttribute("http://www.adambaker.org/gloss.php","baseline-writing-system", mBaselineWritingSystem.flexString() );

    if( !mName.isEmpty() )
        writeItem( "title" , mBaselineWritingSystem , mName , stream );
    if( !mComment.isEmpty() )
        writeItem( "comment" , mBaselineWritingSystem , mComment , stream );

    stream->writeStartElement("paragraphs");

    QProgressDialog progress(tr("Saving text %1...").arg(mName), "Cancel", 0, mPhrases.count(), 0);
    progress.setWindowModality(Qt::WindowModal);

    for(int i=0; i<mPhrases.count(); i++)
    {
        progress.setValue(i);
        if(progress.wasCanceled())
            return false;

        // this rather profligate nesting seems to be a feature of flextext files
        stream->writeStartElement("paragraph");
        stream->writeStartElement("phrases");
        stream->writeStartElement("phrase");
        writeItem("segnum", mBaselineWritingSystem, QString("%1").arg(i+1) , stream );
        stream->writeStartElement("words");
        for(int j=0; j<mPhrases.at(i)->count(); j++)
        {
            stream->writeStartElement("word");

            stream->writeAttribute("http://www.adambaker.org/gloss.php", "id", QString("%1").arg(mPhrases.at(i)->at(j)->id()) );
            if( mPhrases.at(i)->at(j)->approvalStatus() == GlossItem::Approved )
                stream->writeAttribute("http://www.adambaker.org/gloss.php", "approval-status", "true" );
            else
                stream->writeAttribute("http://www.adambaker.org/gloss.php", "approval-status", "false" );


            TextBitHashIterator textIter(*mPhrases.at(i)->at(j)->textForms());
            while (textIter.hasNext())
            {
                textIter.next();
                writeItem("txt",textIter.key(),textIter.value().text() ,stream , textIter.value().id() );
            }

            TextBitHashIterator glossIter(*mPhrases.at(i)->at(j)->glosses());
            while (glossIter.hasNext())
            {
                glossIter.next();
                writeItem("gls",glossIter.key(),glossIter.value().text(),stream, glossIter.value().id());
            }
            stream->writeEndElement(); // word
        }

        stream->writeEndElement(); // words

        // phrase-level glosses
        TextBitHashIterator iter = mPhrases.at(i)->glosses();
        while (iter.hasNext())
        {
            iter.next();
            writeItem("gls",iter.key(),iter.value().text(),stream);
        }

        stream->writeEndElement(); // phrase
        stream->writeEndElement(); // phrases
        stream->writeEndElement(); // paragraph
    }

    progress.setValue(mPhrases.count());

    stream->writeEndElement(); // paragraphs

    stream->writeStartElement("languages");
    QList<WritingSystem> ws = mProject->dbAdapter()->writingSystems();
    for( int i=0; i<ws.count(); i++ )
    {
        stream->writeStartElement("language");
        stream->writeAttribute("lang", ws.at(i).flexString() );
        stream->writeAttribute("font", ws.at(i).fontFamily() );
        stream->writeAttribute("http://www.adambaker.org/gloss.php","font-size", QString("%1").arg(ws.at(i).fontSize()) );
        if( ws.at(i).layoutDirection() == Qt::RightToLeft )
            stream->writeAttribute("RightToLeft", "true" );
        if( mBaselineWritingSystem.flexString() == ws.at(i).flexString() )
            stream->writeAttribute("http://www.adambaker.org/gloss.php","is-baseline", "true" );

        stream->writeEndElement(); // language
    }
    stream->writeEndElement(); // languages

    stream->writeEndElement(); // interlinear-text

    return true;
}

void Text::writeItem(const QString & type, const WritingSystem & ws, const QString & text , QXmlStreamWriter *stream, qlonglong id) const
{
    stream->writeStartElement("item");
    if( id != -1 )
        stream->writeAttribute("http://www.adambaker.org/gloss.php", "id", QString("%1").arg(id) );
    stream->writeAttribute("type",type);
    stream->writeAttribute("lang",ws.flexString());
    stream->writeCharacters(text);
    stream->writeEndElement();
}

void Text::saveText() const
{
    serialize( mProject->filepathFromName(mName) );
}

bool Text::isValid() const
{
    return mValid;
}

bool Text::mergeTranslation(const QString & filename, const WritingSystem & ws )
{
    QString currentPath = mProject->filepathFromName(mName);
    QString tempOutputPath = mProject->filepathFromName(mName + "-output");
    QString errorOutputPath = QDir::temp().absoluteFilePath("merge-error.txt");

    QHash<QString,QString> parameters;
    parameters.insert("flextext-with-translation", filename );
    parameters.insert("writing-system", ws.flexString() );

    Xsltproc transformation;
    transformation.setStyleSheet( QDir::current().absoluteFilePath("merge-translation-by-line-number.xsl") );
    transformation.setXmlFilename( currentPath );
    transformation.setErrorFilename( errorOutputPath );
    transformation.setOutputFilename( tempOutputPath );
    transformation.setParameters(parameters);
    Xsltproc::ReturnValue retval = transformation.execute();

    QFileInfo errorInfo(errorOutputPath);

    switch(retval)
    {
    case Xsltproc::InvalidStylesheet:
        QMessageBox::critical(0, tr("Error"), tr("The XSL transformation is invalid."));
        return false;
    case Xsltproc::InvalidXmlFile:
        QMessageBox::critical(0, tr("Error"), tr("The flextext file is invalid."));
        return false;
    case Xsltproc::GenericFailure:
        if( errorInfo.size() > 0 )
            QDesktopServices::openUrl(QUrl(errorOutputPath, QUrl::TolerantMode));
        else
            QMessageBox::critical(0, tr("Error"), tr("Failure for unknown reasons."));
        QFile::remove(tempOutputPath);
        return false;
    case Xsltproc::Success:
        break;
    }

    if( errorInfo.size() > 0 )
        QDesktopServices::openUrl(QUrl(errorOutputPath, QUrl::TolerantMode));

    if( QFile::remove(currentPath) )
    {
        if( QFile::rename(tempOutputPath, currentPath) )
            QMessageBox::information(0, tr("Success!"), tr("The merge has completed succesfully."));
        else
            QMessageBox::warning(0, tr("Error"), tr("The merge file is stuck with the filename %1, but you can fix this yourself. The old flextext file has been deleted.").arg(tempOutputPath));
    }
    else
    {
        QMessageBox::warning(0, tr("Error"), tr("The old flextext file could not be deleted, so the merge file is stuck with the filename %1, but you can fix this yourself.").arg(tempOutputPath));
    }

    return true;
}

QString Text::textNameFromPath(const QString & path) const
{
    QFileInfo info(path);
    return info.baseName();
}
