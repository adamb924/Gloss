#include "text.h"

#include <QXmlStreamReader>
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
    qlonglong oldId = bit->id();
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
    emit idChanged(bit,oldId);
}

void Text::importTextFromFlexText(QFile *file, bool baselineInfoFromFile)
{
    file->open(QFile::ReadOnly);

    clearTextBits();

    bool inWord = false;
    QXmlStreamReader stream(file);
    while (!stream.atEnd())
    {
        stream.readNext();
        QString name = stream.name().toString();
        if( stream.tokenType() == QXmlStreamReader::StartElement )
        {
            if( name == "word" )
            {
                inWord = true;
            }
            else if ( name == "phrase" )
            {
                mBaselineBits.append( new QList<TextBit*> );
            }
            else if ( name == "item" )
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("type") )
                {
                    QString type = attr.value("type").toString();
                    if( type == "txt" )
                    {

                    }
                    else if( type == "gls" )
                    {

                    }
                }
            }
        }
        else if( stream.tokenType() == QXmlStreamReader::EndElement && name == "word" )
        {
            inWord = false;
        }
    }
    if (stream.hasError()) {
        qDebug() << "Text::readTextFromFlexText error with xml reading";
    }
}
