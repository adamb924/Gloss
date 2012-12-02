#include "text.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QTextStream>
#include <QtDebug>
#include <QHashIterator>

#include "writingsystem.h"
#include "project.h"
#include "databaseadapter.h"
#include "phrase.h"

Text::Text()
{
}

Text::Text(const QString & name, WritingSystem *ws, Project *project)
{
    mName = name;
    mBaselineWritingSystem = ws;
    mProject = project;
}

Text::Text(QFile *file, Project *project)
{
    mProject = project;
    importTextFromFlexText(file,true);
}

Text::Text(QFile *file, WritingSystem *ws, Project *project)
{
    mProject = project;
    mBaselineWritingSystem = ws;
    importTextFromFlexText(file,false);
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


WritingSystem* Text::writingSystem() const
{
    return mBaselineWritingSystem;
}

void Text::setWritingSystem(WritingSystem *ws)
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

void Text::importTextFromFlexText(QFile *file, bool baselineInfoFromFile)
{
    file->open(QFile::ReadOnly);

    clearGlossItems();

    bool inWord = false;
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
                mGlossItems.append( new Phrase );
            }
            else if ( name == "item" )
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("type") && attr.hasAttribute("lang") )
                {
                    QString type = attr.value("type").toString();
                    WritingSystem *lang = mProject->dbAdapter()->writingSystem( attr.value("lang").toString() );
                    QString text = stream.readElementText();
                    if( type == "txt" )
                    {
                        textForms.append( TextBit( text , lang ) );
                        if( lang->flexString() == mBaselineWritingSystem->flexString() )
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
        else if( stream.tokenType() == QXmlStreamReader::EndElement && name == "word" )
        {
            inWord = false;
            mGlossItems.last()->append(new GlossItem(new TextBit(baselineText,mBaselineWritingSystem)));
            guessInterpretation(mGlossItems.last()->last(), textForms, glossForms);
        }
    }
    if (stream.hasError()) {
        qDebug() << "Text::readTextFromFlexText error with xml reading";
    }
}

bool Text::serialize(const QString & filename) const
{
    QFile outFile(filename);
    if( !outFile.open(QFile::WriteOnly) )
        return false;

    QXmlStreamWriter stream(&outFile);
    stream.setCodec("UTF-8");
    stream.setAutoFormatting(true);
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

    if( !mName.isEmpty() )
        writeItem( "title" , mBaselineWritingSystem , mName , stream );
    if( !mComment.isEmpty() )
        writeItem( "comment" , mBaselineWritingSystem , mComment , stream );

    stream->writeStartElement("paragraphs");
    stream->writeStartElement("paragraph");
    stream->writeStartElement("phrases");

    for(int i=0; i<mGlossItems.count(); i++)
    {
        stream->writeStartElement("phrase");
        writeItem("segnum", mBaselineWritingSystem, QString("%1").arg(i+1) , stream );
        stream->writeStartElement("words");
        for(int j=0; j<mGlossItems.at(i)->count(); j++)
        {

        }

        stream->writeEndElement(); // words

        // phrase-level glosses
        QHashIterator<WritingSystem*, QString> iter(mGlossItems.at(iter)->glosses());
        while (iter.hasNext())
        {
            iter.next();
            writeItem("gls",iter.key(),iter.value(),stream);
        }

        stream->writeEndElement(); // phrase
    }


    stream->writeEndElement(); // phrases
    stream->writeEndElement(); // paragraph
    stream->writeEndElement(); // paragraphs

    return true;
}

void Text::writeItem(const QString & type, const WritingSystem * ws, const QString & text , QXmlStreamWriter *stream) const
{
    stream->writeStartElement("item");
    stream->writeAttribute("type",type);
    stream->writeAttribute("lang",ws->flexString());
    stream->writeCharacters(mName);
    stream->writeEndElement();
}
