#include "flextextimporter.h"

#include "phrase.h"
#include "text.h"
#include "glossitem.h"
#include "morphologicalanalysis.h"
#include "databaseadapter.h"
#include "project.h"
#include "paragraph.h"

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
            else if ( name == "paragraph" ) // <paragraph>
            {
                mText->mParagraphs.append( new Paragraph );
                QObject::connect( mText->mParagraphs.last(), SIGNAL(changed()), mText, SLOT(markAsChanged()) );
            }
            else if ( name == "phrase" )
            {
                inPhrase = true;
                mText->mParagraphs.last()->mPhrases.append( new Phrase(mText, mText->mProject) );
                mText->mParagraphs.last()->mPhrases.last()->connectToText();

                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","annotation-start") && attr.hasAttribute("http://www.adambaker.org/gloss.php","annotation-end") )
                {
                    qlonglong start = attr.value("http://www.adambaker.org/gloss.php","annotation-start").toString().toLongLong();
                    qlonglong end = attr.value("http://www.adambaker.org/gloss.php","annotation-end").toString().toLongLong();
                    mText->mParagraphs.last()->phrases()->last()->setInterval( Interval(start, end) );
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
                        mText->mParagraphs.last()->phrases()->last()->setPhrasalGloss( TextBit( text , lang ) );
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
                mText->mParagraphs.last()->phrases()->last()->appendGlossItem(new GlossItem( mText->mBaselineWritingSystem, textForms, glossForms, mText->mProject ));
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

    return FlexTextImporter::FlexTextReadSuccess;
}
