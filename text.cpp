#include "text.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QTextStream>
#include <QtDebug>
#include <QHashIterator>
#include <QXmlQuery>

#include "writingsystem.h"
#include "project.h"
#include "databaseadapter.h"
#include "phrase.h"
#include "glossitem.h"
#include "messagehandler.h"

Text::Text()
{
}

Text::Text(const WritingSystem & ws, const QString & name, Project *project)
{
    mName = name;
    mBaselineWritingSystem = ws;
    mProject = project;
    mValid = true;
}

Text::Text(const QString & filePath, Project *project)
{
    qDebug() << "Text::Text(const QString & filePath, Project *project)";
    QFile *file = new QFile(filePath);
    mProject = project;
    mValid = importTextFromFlexText(file,true);
}

Text::Text(const QString & filePath, const WritingSystem & ws, Project *project)
{
    QFile *file = new QFile(filePath);
    mProject = project;
    mValid = importTextFromFlexText(file,false);
    mBaselineWritingSystem = ws;
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


WritingSystem Text::writingSystem() const
{
    return mBaselineWritingSystem;
}

void Text::setWritingSystem(const WritingSystem & ws)
{
    mBaselineWritingSystem = ws;
}

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

QList<Phrase*>* Text::glossItems()
{
    return &mGlossItems;
}

void Text::clearGlossItems()
{
    Phrase *phrase;
    foreach( phrase , mGlossItems )
    {
        qDeleteAll(*phrase);
        phrase->clear();
    }
    qDeleteAll(mGlossItems);
    mGlossItems.clear();
}

void Text::setGlossItemsFromBaseline()
{
    QStringList lines = mBaselineText.split(QRegExp("[\\n\\r]+"),QString::SkipEmptyParts);
    for(int i=0; i<lines.count(); i++)
    {
        mGlossItems.append( new Phrase );
        QStringList words = lines.at(i).split(QRegExp("[ \\t]+"),QString::SkipEmptyParts);
        for(int j=0; j<words.count(); j++)
        {
            mGlossItems.last()->append(new GlossItem(new TextBit(words.at(j),mBaselineWritingSystem)));
            guessInterpretation(mGlossItems.last()->last());
        }
    }
}

void Text::guessInterpretation(GlossItem *item)
{
    QList<qlonglong> candidates =  mProject->dbAdapter()->candidateInterpretations( *item->baselineText() );
    if( candidates.length() == 0 )
    {
        item->setId( mProject->dbAdapter()->newInterpretation(*item->baselineText()) );
        mCandidateStatus = SingleOption;
    }
    else if ( candidates.length() == 1 )
    {
        item->setId( candidates.at(0) );
        mCandidateStatus = SingleOption;
    }
    else // greater than 1
    {
        item->setId( candidates.at(0) );
        mCandidateStatus = MultipleOption;
    }
    mApprovalStatus = Unapproved;
}

void Text::guessInterpretation(GlossItem *item, const QList<TextBit> & textForms, const QList<TextBit> & glossForms)
{
    QList<qlonglong> candidates = mProject->dbAdapter()->candidateInterpretations(textForms,glossForms);
    if( candidates.length() == 0 )
    {
        qlonglong id = mProject->dbAdapter()->newInterpretation(textForms,glossForms);
        item->setId( id );
        mCandidateStatus = SingleOption;
    }
    else if ( candidates.length() == 1 )
    {
        item->setId( candidates.at(0) );
        mCandidateStatus = SingleOption;
    }
    else // greater than 1
    {
        item->setId( candidates.at(0) );
        mCandidateStatus = MultipleOption;
    }
}

bool Text::setBaselineWritingSystemFromFile(const QString & filePath )
{
    qDebug( ) << filePath;

    QXmlQuery query(QXmlQuery::XQuery10);
    query.setFocus(QUrl(filePath));
    query.setMessageHandler(new MessageHandler(this));
    query.setQuery("declare namespace abg = \"http://www.adambaker.org/gloss.php\"; for $x in /document/interlinear-text/languages/language[@abg:is-baseline='true'] return string($x/@lang)");
    if (query.isValid())
    {
        QStringList result;
        query.evaluateTo(&result);
        qDebug() << result;

        if( result.isEmpty() )
            return false;

        mBaselineWritingSystem = mProject->dbAdapter()->writingSystem( result.at(0) );

        return false; // change
    }
    else
    {
        qDebug() << "Invalid query";
        return false;
    }
}

bool Text::importTextFromFlexText(QFile *file, bool baselineInfoFromFile)
{
    qDebug() << baselineInfoFromFile;
    if( baselineInfoFromFile)
    {
        if( !setBaselineWritingSystemFromFile(file->fileName()) )
            return false;
    }

    qDebug() << "Text::importTextFromFlexText writing system id:" << mBaselineWritingSystem.id();

    file->open(QFile::ReadOnly);

    QFileInfo info(file->fileName());
    mName = info.baseName();

    clearGlossItems();

    bool inWord = false;
    bool inPhrase = false;
    QXmlStreamReader stream(file);
    QList<TextBit> textForms;
    QList<TextBit> glossForms;
    QString baselineText;

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
            }
            else if ( name == "phrase" )
            {
                inPhrase = true;
                mGlossItems.append( new Phrase );
            }
            else if ( name == "item" )
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("type") && attr.hasAttribute("lang") )
                {
                    QString type = attr.value("type").toString();
                    WritingSystem lang = mProject->dbAdapter()->writingSystem( attr.value("lang").toString() );
                    QString text = stream.readElementText();
                    if( type == "txt" )
                    {
//                        qDebug() << lang->flexString();
//                        qDebug() << mBaselineWritingSystem.flexString();
                        textForms.append( TextBit( text , lang ) );
                        if( lang.flexString() == mBaselineWritingSystem.flexString() )
                        {
                            baselineText = text;
                        }
                    }
                    else if( type == "gls" )
                    {
                        glossForms.append( TextBit( text , lang ) );
                    }
                }
            }
        }
        else if( stream.tokenType() == QXmlStreamReader::EndElement )
        {
            if(name == "word")
            {
                inWord = false;
                mGlossItems.last()->append(new GlossItem(new TextBit(baselineText, mBaselineWritingSystem), textForms, glossForms));
                guessInterpretation(mGlossItems.last()->last(), textForms, glossForms);
            }
            else if(name == "phrase")
            {
                inPhrase = false;
            }
        }
    }
    if (stream.hasError()) {
        qDebug() << "Text::readTextFromFlexText error with xml reading";
        return false;
    }
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

    for(int i=0; i<mGlossItems.count(); i++)
    {
        // this rather profligate nesting seems to be a feature of flextext files
        stream->writeStartElement("paragraph");
        stream->writeStartElement("phrases");
        stream->writeStartElement("phrase");
        writeItem("segnum", mBaselineWritingSystem, QString("%1").arg(i+1) , stream );
        stream->writeStartElement("words");
        for(int j=0; j<mGlossItems.at(i)->count(); j++)
        {
            stream->writeStartElement("word");

            stream->writeAttribute("http://www.adambaker.org/gloss.php", "id", QString("%1").arg(mGlossItems.at(i)->at(j)->id()) );

            QHashIterator<WritingSystem, QString> textIter(*mGlossItems.at(i)->at(j)->textItems());
            while (textIter.hasNext())
            {
                textIter.next();
                writeItem("txt",textIter.key(),textIter.value(),stream);
            }

            QHashIterator<WritingSystem, QString> glossIter(*mGlossItems.at(i)->at(j)->glossItems());
            while (glossIter.hasNext())
            {
                glossIter.next();
                writeItem("gls",glossIter.key(),glossIter.value(),stream);
            }
            stream->writeEndElement(); // word
        }

        stream->writeEndElement(); // words

        // phrase-level glosses
        QHashIterator<WritingSystem, QString> iter(*mGlossItems.at(i)->glosses());
        while (iter.hasNext())
        {
            iter.next();
            writeItem("gls",iter.key(),iter.value(),stream);
        }

        stream->writeEndElement(); // phrase
        stream->writeEndElement(); // phrases
        stream->writeEndElement(); // paragraph
    }


    stream->writeEndElement(); // paragraphs

    stream->writeStartElement("languages");
    QList<WritingSystem*> ws = mProject->dbAdapter()->writingSystems();
    for( int i=0; i<ws.count(); i++ )
    {
        stream->writeStartElement("language");
        stream->writeAttribute("lang", ws.at(i)->flexString() );
        stream->writeAttribute("font", ws.at(i)->fontFamily() );
        stream->writeAttribute("http://www.adambaker.org/gloss.php","font-size", QString("%1").arg(ws.at(i)->fontSize()) );
        if( ws.at(i)->layoutDirection() == Qt::RightToLeft )
            stream->writeAttribute("RightToLeft", "true" );
        if( mBaselineWritingSystem.flexString() == ws.at(i)->flexString() )
            stream->writeAttribute("http://www.adambaker.org/gloss.php","is-baseline", "true" );

        stream->writeEndElement(); // language
    }
    stream->writeEndElement(); // languages

    stream->writeEndElement(); // interlinear-text

    return true;
}

void Text::writeItem(const QString & type, const WritingSystem & ws, const QString & text , QXmlStreamWriter *stream) const
{
    stream->writeStartElement("item");
    stream->writeAttribute("type",type);
    stream->writeAttribute("lang",ws.flexString());
    stream->writeCharacters(text);
    stream->writeEndElement();
}

void Text::saveText(QDir tempDir) const
{
    serialize( tempDir.absoluteFilePath( QString("%1.flextext").arg(mName) ) );
}

bool Text::isValid() const
{
    return mValid;
}
