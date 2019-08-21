#include "flextextreader.h"

#include "phrase.h"
#include "text.h"
#include "glossitem.h"
#include "morphologicalanalysis.h"
#include "databaseadapter.h"
#include "project.h"
#include "messagehandler.h"
#include "syntacticanalysis.h"
#include "syntacticanalysiselement.h"
#include "paragraph.h"

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

    mText->removeAllParagraphs();

    bool inWord = false;
    bool inPhrase = false;
    bool inMorphemes = false;

    qlonglong interpretationId = -1;
    GlossItem::ApprovalStatus approvalStatus = GlossItem::Unapproved;
    WritingSystem glossItemBaselineWritingSystem;

    QHash<QString,Annotation> annotations;
    TextBit annotationHeader, annotationText;
    QString annotationKey;
    QSet<qlonglong> textFormIds;
    QSet<qlonglong> glossFormIds;
    QHash<QString,QList<QUuid> > morphGuids; /// the string here is a flextext string
    QList<QUuid> *currentGuids = nullptr;
    SyntacticAnalysis * currentSyntacticAnalysis = nullptr;
    QStack<SyntacticAnalysisElement*> elementStack;

    while (!stream.atEnd())
    {
        stream.readNext();
        QString name = stream.name().toString();
        QString nameSpace = stream.namespaceUri().toString();
        if( stream.tokenType() == QXmlStreamReader::StartElement )
        {
            if( name == "word" ) // <word>
            {
                inWord = true;

                textFormIds.clear();
                glossFormIds.clear();
                annotations.clear();

                morphGuids.clear();

                if( stream.attributes().hasAttribute("http://www.adambaker.org/gloss.php","id") )
                {
                    interpretationId = stream.attributes().value("http://www.adambaker.org/gloss.php","id").toString().toLongLong();
                    if( mDbAdapter->interpretationExists(interpretationId) )
                    {
                        if(stream.attributes().hasAttribute("http://www.adambaker.org/gloss.php", "approval-status") && stream.attributes().value("http://www.adambaker.org/gloss.php", "approval-status").toString() == "true" )
                            approvalStatus = GlossItem::Approved;
                    }
                }

                if( stream.attributes().hasAttribute("http://www.adambaker.org/gloss.php","baseline-writing-system") )
                {
                    glossItemBaselineWritingSystem = mDbAdapter->writingSystem( stream.attributes().value("http://www.adambaker.org/gloss.php", "baseline-writing-system").toString() );
                }
            }
            else if ( name == "paragraph" ) // <paragraph>
            {
                mText->mParagraphs.append( new Paragraph(mText->mBaselineWritingSystem) );
                QObject::connect( mText->mParagraphs.last(), SIGNAL(changed()), mText, SLOT(markAsChanged()) );

                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","title") )
                {
                    mText->mParagraphs.last()->setHeader( TextBit( attr.value("http://www.adambaker.org/gloss.php","title").toString(), mText->mBaselineWritingSystem ) );
                }
            }
            else if ( name == "phrase" ) // <phrase>
            {
                inPhrase = true;

                mText->mParagraphs.last()->mPhrases.append( new Phrase(mText, mText->mProject) );
                mText->mParagraphs.last()->mPhrases.last()->connectToText();

                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","annotation-start") && attr.hasAttribute("http://www.adambaker.org/gloss.php","annotation-end") )
                {
                    qlonglong start = attr.value("http://www.adambaker.org/gloss.php","annotation-start").toString().toLongLong();
                    qlonglong end = attr.value("http://www.adambaker.org/gloss.php","annotation-end").toString().toLongLong();
                    mText->mParagraphs.last()->mPhrases.last()->setInterval( Interval(start, end) );
                }
            }
            else if ( name == "item" && !inMorphemes ) // <item>
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
                        if( type == "txt" && itemId != -1 && lang.isValid() )
                        {
                            textFormIds.insert( itemId );
                        }
                        else if( type == "gls" && itemId != -1 && lang.isValid() )
                        {
                            glossFormIds.insert( itemId );
                        }
                        else if ( type == "punct" && itemId != -1 && lang.isValid() )
                        {
                            textFormIds.insert( itemId );
                        }
                    }
                    else if ( inPhrase && type == "gls" && lang.isValid() )
                    {
                        mText->mParagraphs.last()->mPhrases.last()->setPhrasalGloss( TextBit( text , lang ) );
                    }
                }
            }
            else if ( name == "annotation" && stream.namespaceUri().toString() == "http://www.adambaker.org/gloss.php" ) // <annotation>
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("key") )
                {
                    annotationKey = attr.value("key").toString();
                }
                if( attr.hasAttribute("lang") ) // if it has this attribute, it's an old-style annotation
                {
                    WritingSystem ws = mDbAdapter->writingSystem( attr.value("lang").toString() );
                    QString text = stream.readElementText();
                    annotationHeader = TextBit( "", ws ); // blank header by default, the text's WritingSystem by default
                    annotationText = TextBit( text, ws );
                    // in the old format, the close tag won't be read
                    annotations.insert( annotationKey , Annotation( annotationHeader, annotationText ) );
                }
            }
            else if ( name == "annotation-header" && stream.namespaceUri().toString() == "http://www.adambaker.org/gloss.php" ) // <annotation>
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("lang") )
                {
                    WritingSystem ws = mDbAdapter->writingSystem( attr.value("lang").toString() );
                    QString text = stream.readElementText();
                    annotationHeader = TextBit( text, ws );
                }
            }
            else if ( name == "annotation-text" && stream.namespaceUri().toString() == "http://www.adambaker.org/gloss.php" ) // <annotation>
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("lang") )
                {
                    WritingSystem ws = mDbAdapter->writingSystem( attr.value("lang").toString() );
                    QString text = stream.readElementText();
                    annotationText = TextBit( text, ws );
                }
            }
            else if(name == "interlinear-text") // <interlinear-text>
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","audio-file") )
                {
                    mText->setSound( QUrl::fromEncoded( attr.value("http://www.adambaker.org/gloss.php","audio-file").toString().toUtf8() ) );
                }
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","most-recent-line") )
                {
                    mText->mMostRecentLine = attr.value("http://www.adambaker.org/gloss.php","most-recent-line").toInt();
                }
            }
            else if( name == "morphemes") // <morphemes>
            {
                inMorphemes = true;
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","lang") )
                {
                    QString lang = attr.value("http://www.adambaker.org/gloss.php","lang").toString();
                    morphGuids.insert( lang , QList<QUuid>() );
                    currentGuids = & morphGuids[lang];
                }
            }
            else if( name == "morph") // <morph>
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","guid") && !morphGuids.isEmpty() )
                {
                    currentGuids->append( QUuid( attr.value("http://www.adambaker.org/gloss.php","guid").toString() ) );
                }
            }
            else if ( name == "syntactic-analysis" && nameSpace == "http://www.adambaker.org/gloss.php" ) // <syntactic-analysis>
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","name") && attr.hasAttribute("http://www.adambaker.org/gloss.php","lang") )
                {
                    QString name = attr.value("http://www.adambaker.org/gloss.php","name").toString();
                    WritingSystem ws = mDbAdapter->writingSystem( attr.value( "http://www.adambaker.org/gloss.php","lang").toString() );
                    bool closedVocabulary = attr.hasAttribute("http://www.adambaker.org/gloss.php", "closed-vocabulary") && attr.value("http://www.adambaker.org/gloss.php", "closed-vocabulary").toString() == "yes" ? true : false;
                    currentSyntacticAnalysis = new SyntacticAnalysis(name, ws, mText, closedVocabulary );
                    QObject::connect( currentSyntacticAnalysis, SIGNAL(modified()), mText, SLOT(markAsChanged()) );
                    mText->syntacticAnalyses()->insert(name, currentSyntacticAnalysis);
                }
            }
            else if ( name == "constituent" && nameSpace == "http://www.adambaker.org/gloss.php" ) // <constituent>
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","label") )
                {
                    QString label = attr.value("http://www.adambaker.org/gloss.php","label").toString();
                    SyntacticAnalysisElement * newElement;
                    if( currentSyntacticAnalysis->closedVocabulary() )
                    {
                        newElement = new SyntacticAnalysisElement( mDbAdapter->syntacticType(label), QList<SyntacticAnalysisElement*>(), mDbAdapter );
                    }
                    else
                    {
                        newElement = new SyntacticAnalysisElement( SyntacticType(label), QList<SyntacticAnalysisElement*>(), mDbAdapter );
                    }
                    if( elementStack.isEmpty() )
                    {
                        currentSyntacticAnalysis->addBaselineElement(newElement);
                    }
                    else
                    {
                        elementStack.top()->addChild(newElement);
                    }
                    elementStack.push( newElement );
                }
            }
            else if ( name == "terminal" && nameSpace == "http://www.adambaker.org/gloss.php" )  // <terminal>
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","guid") )
                {
                    QString guid = attr.value("http://www.adambaker.org/gloss.php","guid").toString();
                    SyntacticAnalysisElement * newElement = currentSyntacticAnalysis->elementFromGuid( QUuid(guid) );
                    if( !elementStack.isEmpty() )
                    {
                        elementStack.top()->addChild(newElement);
                    }
                }
            }
            else if ( name == "document" )  // <document>
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","export-filename") )
                {
                    mText->setExportFilename( attr.value("http://www.adambaker.org/gloss.php","export-filename").toString() );
                }
            }
        }
        else if( stream.tokenType() == QXmlStreamReader::EndElement )
        {
            if ( name == "annotation" && stream.namespaceUri().toString() == "http://www.adambaker.org/gloss.php" ) // <annotation>
            {
                annotations.insert( annotationKey , Annotation( annotationHeader, annotationText ) );
            }
            else if(name == "word") // </word>
            {
                GlossItem * glossItem = new GlossItem( glossItemBaselineWritingSystem.isNull() ? mText->mBaselineWritingSystem : glossItemBaselineWritingSystem, textFormIds, glossFormIds, interpretationId, mText->mProject );
                mText->mParagraphs.last()->mPhrases.last()->appendGlossItem(glossItem);

                glossItem->setApprovalStatus(approvalStatus);

                glossItem->loadMorphologicalAnalysesFromDatabase();

                QHashIterator<QString,Annotation> annIter(annotations);
                while(annIter.hasNext())
                {
                    annIter.next();
                    glossItem->setAnnotation( annIter.key(), annIter.value() );
                }

                // put all of the GUIDs in for the allomorphs
                foreach( QString lang , morphGuids.keys() )
                {
                    MorphologicalAnalysis * ma = glossItem->morphologicalAnalysis( mDbAdapter->writingSystem(lang) );
                    if( ma->allomorphCount() == morphGuids.value(lang).count() )
                    {
                        for(int i=0; i<ma->allomorphCount(); i++)
                        {
                            ma->allomorph(i)->setGuid( morphGuids.value(lang).at(i) );
                        }
                    }
                }

                inWord = false;
                interpretationId = -1;
                approvalStatus = GlossItem::Unapproved;
                glossItemBaselineWritingSystem = WritingSystem();
            }
            else if(name == "phrase") // </phrase>
            {
                inPhrase = false;
            }
            else if(name == "morphemes") // </morphemes>
            {
                inMorphemes=false;
            }
            else if ( name == "constituent" && nameSpace == "http://www.adambaker.org/gloss.php" ) // </constituent>
            {
                elementStack.pop();
            }
        }
    }

    file.close();

    if (stream.hasError()) {
        qWarning() << "Text::readTextFromFlexText error with xml reading";
        return FlexTextReadXmlReadError;
    }

    return FlexTextReadSuccess;
}

bool FlexTextReader::setBaselineWritingSystemFromFile(const QString & filePath )
{
    QXmlQuery query(QXmlQuery::XQuery10);
    query.setMessageHandler(new MessageHandler("FlexTextReader::setBaselineWritingSystemFromFile" ));
    query.bindVariable("path", QVariant(QUrl::fromLocalFile(filePath).path(QUrl::FullyEncoded)));
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
