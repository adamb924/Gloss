#include "flextextimporter.h"

#include "text.h"
#include "glossitem.h"
#include "morphologicalanalysis.h"
#include "databaseadapter.h"
#include "project.h"

#include <QFile>
#include <QFileInfo>
#include <QtGlobal>
#include <QXmlStreamReader>
#include <QList>
#include <QMessageBox>

FlexTextImporter::FlexTextImporter(Text *text) : FlexTextReader(text)
{
}

FlexTextReader::Result FlexTextImporter::readFile(const QString & filepath)
{
    QFile file(filepath);
    file.open(QFile::ReadOnly);
    QXmlStreamReader stream(&file);

    mText->clearGlossItems();

    bool inWord = false;
    bool inPhrase = false;
    TextBitHash textForms;
    TextBitHash glossForms;

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
                mText->mPhrases.append( new Phrase(mText, mText->mProject) );
                mText->mPhrases.last()->connectToText();

                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","annotation-start") && attr.hasAttribute("http://www.adambaker.org/gloss.php","annotation-end") )
                {
                    qlonglong start = attr.value("http://www.adambaker.org/gloss.php","annotation-start").toString().toLongLong();
                    qlonglong end = attr.value("http://www.adambaker.org/gloss.php","annotation-end").toString().toLongLong();
                    mText->mPhrases.last()->setInterval( Interval(start, end) );
                }
            }
            else if ( name == "item" )
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("type") )
                {
                    QString type = attr.value("type").toString();
                    WritingSystem lang;
                    if( attr.hasAttribute("lang") )
                            lang = mText->mProject->dbAdapter()->writingSystem( attr.value("lang").toString() );
                    QString text = stream.readElementText();

                    if( inWord )
                    {
                        qlonglong itemId = attr.hasAttribute("http://www.adambaker.org/gloss.php","id") ? attr.value("http://www.adambaker.org/gloss.php","id").toString().toLongLong() : -1;
                        // there's no handling here for the case where itemId == -1
                        // that is handled in GlossItem::loadStringsFromDatabase()
                        if( type == "txt" )
                        {
                            TextBit textForm( text , lang, itemId);
                            textForms.insert( lang, textForm );
                        }
                        else if( type == "gls" )
                        {
                            glossForms.insert( lang, TextBit( text , lang, itemId) );
                        }
                    }
                    else if ( inPhrase && type == "gls" )
                    {
                        mText->mPhrases.last()->setPhrasalGloss( TextBit( text , lang ) );
                    }
                }
            }
            else if(name == "interlinear-text")
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","audio-file") )
                {
                    mText->setSound( QUrl::fromEncoded( attr.value("http://www.adambaker.org/gloss.php","audio-file").toString().toUtf8() ) );
                }
            }
        }
        else if( stream.tokenType() == QXmlStreamReader::EndElement )
        {
            if(name == "word")
            {
                mText->mPhrases.last()->appendGlossItem(new GlossItem( mText->mBaselineWritingSystem, textForms, glossForms, mText->mProject ));
                inWord = false;
            }
            else if(name == "phrase")
            {
                inPhrase = false;
            }
        }
    }

    file.close();

    if (stream.hasError()) {
        qWarning() << "FlexTextImporter::readTextFromFlexText error with xml reading";
        return FlexTextImporter::FlexTextReadXmlReadError;
    }
    mText->setBaselineFromGlossItems();

    return FlexTextImporter::FlexTextReadSuccess;
}
