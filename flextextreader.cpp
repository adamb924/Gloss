#include "flextextreader.h"

#include "text.h"
#include "glossitem.h"
#include "morphologicalanalysis.h"
#include "databaseadapter.h"
#include "project.h"
#include "messagehandler.h"

#include <QFile>
#include <QFileInfo>
#include <QtGlobal>
#include <QXmlStreamReader>
#include <QList>
#include <QMessageBox>
#include <QXmlQuery>

FlexTextReader::FlexTextReader(Text *text)
{
    mText = text;
    mDbAdapter = text->mDbAdapter;
}

FlexTextReader::Result FlexTextReader::readFile( const QString & filepath, bool baselineInfoFromFile )
{
    QFile file(filepath);

    if( baselineInfoFromFile)
    {
        if( !setBaselineWritingSystemFromFile(file.fileName()) )
            return FlexTextReadBaselineNotFound;
    }

    file.open(QFile::ReadOnly);
    QXmlStreamReader stream(&file);

    QFileInfo info(file.fileName());
    mText->mName = info.baseName();

    mText->clearGlossItems();

    bool inWord = false;
    bool inPhrase = false;
    bool inMorphemes = false;

    qlonglong interpretationId = -1;
    GlossItem::ApprovalStatus approvalStatus = GlossItem::Unapproved;

    QList<MorphologicalAnalysis*> morphologicalAnalyses;
    MorphologicalAnalysis *morphologicalAnalysis = 0;
    QHash<QString,TextBit> annotations;

    QSet<qlonglong> textFormIds;
    QHash<qlonglong,qlonglong> textFormsByWritingSystem;
    QSet<qlonglong> glossFormIds;

    while (!stream.atEnd())
    {
        stream.readNext();
        QString name = stream.name().toString();
        if( stream.tokenType() == QXmlStreamReader::StartElement )
        {
            if( name == "word" )
            {
                inWord = true;

                textFormIds.clear();
                textFormsByWritingSystem.clear();
                glossFormIds.clear();
                annotations.clear();

                if( stream.attributes().hasAttribute("http://www.adambaker.org/gloss.php","id") )
                {
                    interpretationId = stream.attributes().value("http://www.adambaker.org/gloss.php","id").toString().toLongLong();
                    if( mDbAdapter->interpretationExists(interpretationId) )
                    {
                        if(stream.attributes().hasAttribute("http://www.adambaker.org/gloss.php", "approval-status") && stream.attributes().value("http://www.adambaker.org/gloss.php", "approval-status").toString() == "true" )
                            approvalStatus = GlossItem::Approved;
                    }
                    else
                    {
                        interpretationId = mDbAdapter->newInterpretation();
                    }
                }
                else
                {
                    interpretationId = mDbAdapter->newInterpretation();
                }
            }
            else if ( name == "phrase" )
            {
                inPhrase = true;

                mText->mPhrases.append( new Phrase(mText, mText->mProject) );

                QObject::connect( mText->mPhrases.last(), SIGNAL(phraseChanged()), mText, SLOT(setBaselineFromGlossItems()) );
                QObject::connect( mText->mPhrases.last(), SIGNAL(requestGuiRefresh(Phrase*)), mText, SLOT(requestGuiRefresh(Phrase*)));
                QObject::connect( mText->mPhrases.last(), SIGNAL(glossChanged()), mText, SLOT(markAsChanged()));

                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","annotation-start") && attr.hasAttribute("http://www.adambaker.org/gloss.php","annotation-end") )
                {
                    qlonglong start = attr.value("http://www.adambaker.org/gloss.php","annotation-start").toString().toLongLong();
                    qlonglong end = attr.value("http://www.adambaker.org/gloss.php","annotation-end").toString().toLongLong();
                    mText->mPhrases.last()->setAnnotation( Annotation(start, end) );
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

                    if( inMorphemes )
                    {
                        // TODO do something with this information, eventually
                    }
                    else if( inWord )
                    {
                        qlonglong itemId = attr.hasAttribute("http://www.adambaker.org/gloss.php","id") ? attr.value("http://www.adambaker.org/gloss.php","id").toString().toLongLong() : -1;
                        // there's no handling here for the case where itemId == -1
                        // that is handled in GlossItem::loadStringsFromDatabase()
                        if( type == "txt" && lang.isValid() )
                        {
                            if( itemId == -1 )
                                itemId = mDbAdapter->newTextForm(interpretationId, TextBit( text , lang) );
                            textFormIds.insert(itemId);
                            textFormsByWritingSystem.insert(lang.id(), itemId);

                            MorphologicalAnalysis *analysis = new MorphologicalAnalysis( mDbAdapter->morphologicalAnalysisFromTextFormId(itemId) );
                            if( !analysis->isEmpty() )
                                morphologicalAnalyses << analysis;
                            else
                                delete analysis;
                        }
                        else if( type == "gls" && lang.isValid() )
                        {
                            if( itemId == -1 )
                                itemId = mDbAdapter->newGloss( interpretationId, TextBit( text , lang) );
                            glossFormIds.insert( itemId );
                        }
                    }
                    else if ( inPhrase && type == "gls" && lang.isValid() )
                    {
                        mText->mPhrases.last()->setPhrasalGloss( TextBit( text , lang ) );
                    }
                }
            }
            else if ( name == "annotation" && stream.namespaceUri().toString() == "http://www.adambaker.org/gloss.php" )
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("lang") && attr.hasAttribute("key") )
                {
                    WritingSystem ws = mDbAdapter->writingSystem( attr.value("lang").toString() );
                    QString key = attr.value("key").toString();
                    QString text = stream.readElementText();
                    annotations.insert( key , TextBit(text, ws) );
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
            else if(name == "morphemes")
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","lang") )
                {
                    WritingSystem ws = mDbAdapter->writingSystem( attr.value("http://www.adambaker.org/gloss.php","lang").toString() );
                    if( textFormsByWritingSystem.contains(ws.id()) )
                    {
                        morphologicalAnalysis = new MorphologicalAnalysis( textFormsByWritingSystem.value(ws.id()) , ws );
                        inMorphemes = true;
                    }
                }
            }
            else if(name == "morph")
            {
                if( morphologicalAnalysis != 0 )
                {
                    QXmlStreamAttributes attr = stream.attributes();
                    if( attr.hasAttribute("http://www.adambaker.org/gloss.php","id") )
                        morphologicalAnalysis->addAllomorph( mDbAdapter->allomorphFromId( attr.value("http://www.adambaker.org/gloss.php","id").toString().toLongLong() ) );
                }
            }
        }
        else if( stream.tokenType() == QXmlStreamReader::EndElement )
        {
            if(name == "word")
            {
                mText->mPhrases.last()->appendGlossItem(new GlossItem( mText->mBaselineWritingSystem, textFormIds, glossFormIds, interpretationId, mText->mProject ));
                mText->mPhrases.last()->lastGlossItem()->connectToConcordance();
                mText->mPhrases.last()->lastGlossItem()->setApprovalStatus(approvalStatus);

                QListIterator<MorphologicalAnalysis*> iter(morphologicalAnalyses);
                while(iter.hasNext())
                    mText->mPhrases.last()->lastGlossItem()->setMorphologicalAnalysis( *iter.next() );
                qDeleteAll( morphologicalAnalyses );
                morphologicalAnalyses.clear();

                QHashIterator<QString,TextBit> annIter(annotations);
                while(annIter.hasNext())
                {
                    annIter.next();
                    mText->mPhrases.last()->lastGlossItem()->setAnnotation( annIter.key(), annIter.value() );
                }

                inWord = false;
                interpretationId = -1;
                approvalStatus = GlossItem::Unapproved;
            }
            else if(name == "phrase")
            {
                inPhrase = false;
            }
            else if(name == "morphemes")
            {
                if( !morphologicalAnalysis->isEmpty() )
                    morphologicalAnalyses << morphologicalAnalysis;
                morphologicalAnalysis = 0;
                inMorphemes = false;
            }
        }
    }

    file.close();

    if (stream.hasError()) {
        qWarning() << "Text::readTextFromFlexText error with xml reading";
        return FlexTextReadXmlReadError;
    }
    mText->setBaselineFromGlossItems();
    return FlexTextReadSuccess;
}

bool FlexTextReader::setBaselineWritingSystemFromFile(const QString & filePath )
{
    QXmlQuery query(QXmlQuery::XQuery10);
    query.setMessageHandler(new MessageHandler("FlexTextReader::setBaselineWritingSystemFromFile" ));
    query.bindVariable("path", QVariant(QUrl(filePath).path(QUrl::FullyEncoded)));
    query.setQuery("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                   "declare variable $path external; "
                   "for $x in doc($path)/document/interlinear-text/languages/language[@abg:is-baseline='true'] "
                   "return string($x/@lang)");

    if (query.isValid())
    {
        QStringList result;
        query.evaluateTo(&result);

        if( result.isEmpty() )
            return false;

        mText->mBaselineWritingSystem = mDbAdapter->writingSystem( result.at(0) );

        return true;
    }
    else
    {
        qWarning() << "Text::setBaselineWritingSystemFromFile" << "Invalid query";
        return false;
    }
}
