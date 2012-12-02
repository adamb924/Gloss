#include "text.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QTextStream>
#include <QtDebug>

#include "writingsystem.h"
#include "project.h"
#include "databaseadapter.h"

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
        setBaselineBitsFromBaseline();
        emit baselineTextChanged(mBaselineText);
    }
}

QList< QList<TextBit*>* >* Text::baselineBits()
{
    return &mBaselineBits;
}

void Text::clearTextBits()
{
    QList<TextBit*> *line;
    foreach( line , mBaselineBits )
    {
        qDeleteAll(*line);
        line->clear();
    }
    qDeleteAll(mBaselineBits);
    mBaselineBits.clear();
}

void Text::setBaselineBitsFromBaseline()
{
    QStringList lines = mBaselineText.split(QRegExp("[\\n\\r]+"),QString::SkipEmptyParts);
    for(int i=0; i<lines.count(); i++)
    {
        mBaselineBits.append( new QList<TextBit*> );
        QStringList words = lines.at(i).split(QRegExp("[ \\t]+"),QString::SkipEmptyParts);
        for(int j=0; j<words.count(); j++)
        {
            mBaselineBits.last()->append(new TextBit(words.at(j),mBaselineWritingSystem));
            guessInterpretation(mBaselineBits.last()->last());
        }
    }
}

void Text::guessInterpretation(TextBit *bit)
{
    QList<qlonglong> candidates =  mProject->dbAdapter()->candidateInterpretations( *bit );
//    qlonglong oldId = bit->id();
    if( candidates.length() == 0 )
    {
        bit->setId( mProject->dbAdapter()->newInterpretation(*bit) );
        mCandidateStatus = SingleOption;
    }
    else if ( candidates.length() == 1 )
    {
        bit->setId( candidates.at(0) );
        mCandidateStatus = SingleOption;
    }
    else // greater than 1
    {
        bit->setId( candidates.at(0) );
        mCandidateStatus = MultipleOption;
    }
    mApprovalStatus = Unapproved;
    // TODO rethink what this call ought to be doing
    //    emit idChanged(bit,oldId);
}

void Text::guessInterpretation(TextBit *bit, const QList<TextBit> & textForms, const QList<TextBit> & glossForms)
{
    QList<qlonglong> candidates = mProject->dbAdapter()->candidateInterpretations(textForms,glossForms);
    if( candidates.length() == 0 )
    {
        qlonglong id = mProject->dbAdapter()->newInterpretation(textForms,glossForms);
        bit->setId( id );
        mCandidateStatus = SingleOption;
    }
    else if ( candidates.length() == 1 )
    {
        bit->setId( candidates.at(0) );
        mCandidateStatus = SingleOption;
    }
    else // greater than 1
    {
        bit->setId( candidates.at(0) );
        mCandidateStatus = MultipleOption;
    }
}

void Text::importTextFromFlexText(QFile *file, bool baselineInfoFromFile)
{
    file->open(QFile::ReadOnly);

    clearTextBits();

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
                mBaselineBits.append( new QList<TextBit*> );
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
            mBaselineBits.last()->append(new TextBit(baselineText,mBaselineWritingSystem));
            guessInterpretation(mBaselineBits.last()->last(), textForms, glossForms);
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
    if( !mName.isEmpty() )
        writeItem( "title" , mBaselineWritingSystem , mName , stream );
    if( !mComment.isEmpty() )
        writeItem( "comment" , mBaselineWritingSystem , mComment , stream );

    stream->writeStartElement("paragraphs");
    stream->writeStartElement("paragraph");
    stream->writeStartElement("phrases");

    for(int i=0; i)
    stream->writeStartElement("phrase");

    stream->writeEndElement(); // phrase

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
