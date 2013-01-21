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
#include <QStringList>
#include <QList>

#include "writingsystem.h"
#include "project.h"
#include "databaseadapter.h"
#include "phrase.h"
#include "glossitem.h"
#include "messagehandler.h"
#include "xsltproc.h"
#include "morphologicalanalysis.h"
#include "allomorph.h"
#include "sound.h"

Text::Text()
{
    mSound = 0;
    mReadResult = FlexTextReadNoAttempt;
    mValid = false;
    mChanged = false;
}

Text::Text(const WritingSystem & ws, const QString & name, Project *project)
{
    mSound = 0;
    mName = name;
    mBaselineWritingSystem = ws;
    mProject = project;
    mDbAdapter = mProject->dbAdapter();
    mReadResult = FlexTextReadNoAttempt;
    mValid = true;
    mChanged = false;
}

Text::Text(const QString & filePath, Project *project)
{
    mSound = 0;

    mName = textNameFromPath(filePath);
    mProject = project;
    mDbAdapter = mProject->dbAdapter();

    QFile file(filePath);
    mReadResult = readTextFromFlexText(&file,true);
    if( mReadResult != Text::FlexTextReadSuccess )
        mValid = false;
    mChanged = false;
}

Text::Text(const QString & filePath, const WritingSystem & ws, Project *project)
{
    mSound = 0;
    mName = textNameFromPath(filePath);
    mProject = project;
    mDbAdapter = mProject->dbAdapter();
    mBaselineWritingSystem = ws;

    QFile file(filePath);
    mReadResult = readTextFromFlexText(&file,false);
    if( mReadResult != Text::FlexTextReadSuccess )
        mValid = false;
    mChanged = false;
}

Text::~Text()
{
    qDeleteAll(mPhrases);
    mPhrases.clear();

    if( mSound != 0 )
        delete mSound;
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
            mPhrases.append( new Phrase(this, mProject) );
            connect( mPhrases.last(), SIGNAL(phraseChanged()), this, SLOT(setBaselineFromGlossItems()) );

            setLineOfGlossItems(mPhrases.last(), lines.at(i));
            if( progress.wasCanceled() )
            {
                mValid = false;
                break;
            }
        }
        progress.setValue(lines.count());
    }
//    emit glossItemsChanged();
}

void Text::setLineOfGlossItems( Phrase * phrase , const QString & line )
{
    phrase->clearGlossItems();

    QStringList words = line.split(QRegExp("[ \\t]+"),QString::SkipEmptyParts);
    for(int i=0; i<words.count(); i++)
        phrase->appendGlossItem(new GlossItem(TextBit(words.at(i),mBaselineWritingSystem), mProject ));

    phrase->setGuiRefreshRequest(true);
}

bool Text::setBaselineWritingSystemFromFile(const QString & filePath )
{
    QXmlQuery query(QXmlQuery::XQuery10);
    query.setFocus(QUrl(filePath));
    query.setMessageHandler(new MessageHandler(this));
    query.setQuery("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                   "for $x in /document/interlinear-text/languages/language[@abg:is-baseline='true'] "
                   "return string($x/@lang)");
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
        qWarning() << "Text::setBaselineWritingSystemFromFile" << "Invalid query";
        return false;
    }
}

Text::FlexTextReadResult Text::readTextFromFlexText(QFile *file, bool baselineInfoFromFile)
{
    if( baselineInfoFromFile)
    {
        if( !setBaselineWritingSystemFromFile(file->fileName()) )
            return Text::FlexTextReadBaselineNotFound;
    }

    file->open(QFile::ReadOnly);

    QFileInfo info(file->fileName());
    mName = info.baseName();

    clearGlossItems();

    int lineNumber = -1;
    bool inWord = false;
    bool inPhrase = false;
    bool inMorphemes = false;
    bool hasValidId = false;
    qlonglong id = -1;
    GlossItem::ApprovalStatus approvalStatus = GlossItem::Unapproved;
    QXmlStreamReader stream(file);
    TextBitHash textForms;
    TextBitHash glossForms;
    MorphologicalAnalysis *morphologicalAnalysis = 0;
    WritingSystem maWs;
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
                if( morphologicalAnalysis != 0 )
                {
                    delete morphologicalAnalysis;
                    morphologicalAnalysis = 0;
                }

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
                mPhrases.append( new Phrase(this, mProject) );
                connect( mPhrases.last(), SIGNAL(phraseChanged()), this, SLOT(setBaselineFromGlossItems()) );

                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","annotation-start") && attr.hasAttribute("http://www.adambaker.org/gloss.php","annotation-end") )
                {
                    qlonglong start = attr.value("http://www.adambaker.org/gloss.php","annotation-start").toString().toLongLong();
                    qlonglong end = attr.value("http://www.adambaker.org/gloss.php","annotation-end").toString().toLongLong();
                    mPhrases.last()->setAnnotation( Annotation(start, end) );
                }
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

                    if( inMorphemes )
                    {
                        // TODO do something with this information, eventually
                    }
                    else if( inWord )
                    {
                        qlonglong itemId = attr.hasAttribute("http://www.adambaker.org/gloss.php","id") ? attr.value("http://www.adambaker.org/gloss.php","id").toString().toLongLong() : -1;
                        // there's no handling here for the case where itemId == -1
                        // that is handled in GlossItem::loadStringsFromDatabase()
                        if( type == "txt" )
                        {
                            TextBit textForm( text , lang, itemId);
                            textForms.insert( lang, textForm );
                            if( lang.flexString() == mBaselineWritingSystem.flexString() )
                            {
                                baselineText = text;
                                morphologicalAnalysis = new MorphologicalAnalysis(textForm);
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
            else if(name == "interlinear-text")
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","audio-file") )
                {
                    setSound( QUrl::fromEncoded( attr.value("http://www.adambaker.org/gloss.php","audio-file").toString().toUtf8() ) );
                }
            }
            else if(name == "morphemes")
            {
                QXmlStreamAttributes attr = stream.attributes();
                if( attr.hasAttribute("http://www.adambaker.org/gloss.php","lang") )
                {
                    maWs = mDbAdapter->writingSystem( attr.value("http://www.adambaker.org/gloss.php","lang").toString() );
                    inMorphemes = true;
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

                mPhrases.last()->appendGlossItem(new GlossItem( mBaselineWritingSystem, textForms, glossForms, id, mProject ));
                mPhrases.last()->lastGlossItem()->setApprovalStatus(approvalStatus);

                if( !morphologicalAnalysis->isEmpty() )
                    mPhrases.last()->lastGlossItem()->setMorphologicalAnalysis( *morphologicalAnalysis );

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
            else if(name == "morphemes")
            {
                inMorphemes = false;
            }
        }
    }

    file->close();

    if (stream.hasError()) {
        qWarning() << "Text::readTextFromFlexText error with xml reading";
        return Text::FlexTextReadXmlReadError;
    }
    setBaselineFromGlossItems();
    return Text::FlexTextReadSuccess;
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
    stream->writeStartElement("interlinear-text");

    stream->writeAttribute("http://www.adambaker.org/gloss.php","baseline-writing-system", mDbAdapter->metaLanguage().flexString() );

    if( !mAudioFileURL.isEmpty() )
        stream->writeAttribute("http://www.adambaker.org/gloss.php","audio-file", mAudioFileURL.toString() );

    if( !mName.isEmpty() )
        serializeItem( "title" , mDbAdapter->metaLanguage() , mName , stream );
    if( !mComment.isEmpty() )
        serializeItem( "comment" , mDbAdapter->metaLanguage() , mComment , stream );

    stream->writeStartElement("paragraphs");

    QProgressDialog progress(tr("Saving text %1...").arg(mName), QString(), 0, mPhrases.count(), 0);
    progress.setWindowModality(Qt::WindowModal);

    int count = 0;
    foreach( Phrase* phrase, mPhrases )
    {
        count++;

        progress.setValue(count);

        // this rather profligate nesting seems to be a feature of flextext files
        stream->writeStartElement("paragraph");
        stream->writeStartElement("phrases");
        stream->writeStartElement("phrase");

        if( !phrase->annotation()->isNull() )
        {
            stream->writeAttribute("http://www.adambaker.org/gloss.php","annotation-start", QString("%1").arg(phrase->annotation()->start()) );
            stream->writeAttribute("http://www.adambaker.org/gloss.php","annotation-end", QString("%1").arg(phrase->annotation()->end()) );
        }

        serializeItem("segnum", mDbAdapter->metaLanguage(), QString("%1").arg(count) , stream );

        stream->writeStartElement("words");
        for(int i=0; i<phrase->glossItemCount(); i++ )
        {
            GlossItem *glossItem = phrase->glossItemAt(i);

            serializeGlossItem(glossItem, stream);
        }

        stream->writeEndElement(); // words

        // phrase-level glosses
        TextBitHashIterator iter = phrase->glosses();
        while (iter.hasNext())
        {
            iter.next();
            serializeItem("gls",iter.key(),iter.value().text(),stream);
        }

        stream->writeEndElement(); // phrase
        stream->writeEndElement(); // phrases
        stream->writeEndElement(); // paragraph
    }

    progress.setValue(mPhrases.count());

    stream->writeEndElement(); // paragraphs

    serializeLanguages(stream);

    stream->writeEndElement(); // interlinear-text

    return true;
}

bool Text::serializeGlossItem(GlossItem *glossItem, QXmlStreamWriter *stream) const
{
    stream->writeStartElement("word");

    stream->writeAttribute("http://www.adambaker.org/gloss.php", "id", QString("%1").arg(glossItem->id()) );
    if( glossItem->approvalStatus() == GlossItem::Approved )
        stream->writeAttribute("http://www.adambaker.org/gloss.php", "approval-status", "true" );
    else
        stream->writeAttribute("http://www.adambaker.org/gloss.php", "approval-status", "false" );


    TextBitHashIterator textIter(*glossItem->textForms());

    while (textIter.hasNext())
    {
        textIter.next();
        serializeItem("txt",textIter.key(),textIter.value().text() ,stream , textIter.value().id() );
    }

    TextBitHashIterator glossIter(*glossItem->glosses());

    while (glossIter.hasNext())
    {
        glossIter.next();
        serializeItem("gls",glossIter.key(),glossIter.value().text(),stream, glossIter.value().id());
    }

    serializeMorphemes( glossItem, stream );

    stream->writeEndElement(); // word

    return true;
}

bool Text::serializeMorphemes(GlossItem *glossItem, QXmlStreamWriter *stream) const
{
    QList<WritingSystem> analysisLanguages = glossItem->morphologicalAnalysisLanguages();
    foreach( WritingSystem ws, analysisLanguages )
    {
        // TODO optimize this to use the methods for accessing the morphological analysis instead
        const MorphologicalAnalysis analysis = glossItem->morphologicalAnalysis( ws );
        if( ! analysis.isEmpty() )
        {
            stream->writeStartElement("morphemes");
            stream->writeAttribute("http://www.adambaker.org/gloss.php", "lang", ws.flexString() );

            AllomorphIterator iter = analysis.allomorphIterator();
            while(iter.hasNext())
                serializeAllomorph( iter.next() , stream );
            stream->writeEndElement(); // morphemes
        }
    }
    return true;
}

bool Text::serializeAllomorph(const Allomorph & allomorph, QXmlStreamWriter *stream) const
{
    stream->writeStartElement("morph");
    stream->writeAttribute("type", allomorph.typeString());
    stream->writeAttribute("http://www.adambaker.org/gloss.php", "id", QString("%1").arg(allomorph.id()) );

    serializeItem( "txt" , allomorph.writingSystem(), allomorph.text(), stream );

    TextBitHash citationForms = mDbAdapter->lexicalEntryCitationFormsForAllomorph( allomorph.id() );
    TextBitHashIterator citationIter(citationForms);
    while( citationIter.hasNext() )
    {
        citationIter.next();
        serializeItem( "cf", citationIter.key(), citationIter.value().text(), stream, citationIter.value().id() );
    }

    TextBitHash glossForms = mDbAdapter->lexicalEntryGlossFormsForAllomorph( allomorph.id() );
    TextBitHashIterator glossIter(glossForms);
    while( glossIter.hasNext() )
    {
        glossIter.next();
        serializeItem( "gls", glossIter.key(), glossIter.value().text(), stream, glossIter.value().id() );
    }

    QStringListIterator tagIter( mDbAdapter->grammaticalTagsForAllomorph( allomorph.id() ) );
    while( tagIter.hasNext() )
    {
        stream->writeEmptyElement("http://www.adambaker.org/gloss.php", "grammatical-tag");
        stream->writeAttribute("http://www.adambaker.org/gloss.php", "name", tagIter.next() );
    }

    stream->writeEndElement(); // morph

    return true;
}

bool Text::serializeLanguages(QXmlStreamWriter *stream) const
{
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

    return true;
}

void Text::serializeItem(const QString & type, const WritingSystem & ws, const QString & text , QXmlStreamWriter *stream, qlonglong id) const
{
    stream->writeStartElement("item");
    if( id != -1 )
        stream->writeAttribute("http://www.adambaker.org/gloss.php", "id", QString("%1").arg(id) );
    stream->writeAttribute("type",type);
    stream->writeAttribute("lang",ws.flexString());
    stream->writeCharacters(text);
    stream->writeEndElement();
}

void Text::saveText(bool saveAnyway)
{
    if( mChanged || saveAnyway )
    {
        serialize( mProject->filepathFromName(mName) );
        mChanged = false;
    }
}

bool Text::isValid() const
{
    return mValid;
}

Text::MergeTranslationResult Text::mergeTranslation(const QString & filename, const WritingSystem & ws )
{
    QString currentPath = mProject->filepathFromName(mName);
    QString tempOutputPath = mProject->filepathFromName(mName + "-output");
    QString errorOutputPath = QDir::temp().absoluteFilePath(mName + "merge-error.txt");

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
        QMessageBox::critical(0, tr("Error"), tr("The XSL transformation %1 is invalid.").arg(QDir::current().absoluteFilePath("merge-translation-by-line-number.xsl")));
        return XslTranslationError;
    case Xsltproc::InvalidXmlFile:
        QMessageBox::critical(0, tr("Error"), tr("The flextext file %1 is invalid.").arg(currentPath) );
        return XslTranslationError;
    case Xsltproc::GenericFailure:
        if( errorInfo.size() > 0 )
            QDesktopServices::openUrl(QUrl(errorOutputPath, QUrl::TolerantMode));
        else
            QMessageBox::critical(0, tr("Error"), tr("Failure for unknown reasons."));
        QFile::remove(tempOutputPath);
        return XslTranslationError;
    case Xsltproc::Success:
        break;
    }

    if( errorInfo.size() > 0 )
        QDesktopServices::openUrl(QUrl(errorOutputPath, QUrl::TolerantMode));

    if( QFile::remove(currentPath) )
    {
        if( QFile::rename(tempOutputPath, currentPath) )
        {
            //            QMessageBox::information(0, tr("Success!"), tr("The merge has completed succesfully."));
            return Success;
        }
        else
        {
            //            QMessageBox::warning(0, tr("Error"), tr("The merge file is stuck with the filename %1, but you can fix this yourself. The old flextext file has been deleted.").arg(tempOutputPath));
            return MergeStuckOldFileDeleted;
        }
    }
    else
    {
        //        QMessageBox::warning(0, tr("Error"), tr("The old flextext file could not be deleted, so the merge file is stuck with the filename %1, but you can fix this yourself.").arg(tempOutputPath));
        return MergeStuckOldFileStillThere;
    }
}

Text::MergeEafResult Text::mergeEaf(const QString & filename )
{
    QStringList result;
    QXmlQuery query(QXmlQuery::XQuery10);
    if(!query.setFocus(QUrl( filename )))
        return MergeEafFailure;
    query.setMessageHandler(new MessageHandler());
    query.setQuery( "declare variable $settings-array := /ANNOTATION_DOCUMENT/TIME_ORDER/TIME_SLOT; "
                    "for $phrase in /ANNOTATION_DOCUMENT/TIER/ANNOTATION/ALIGNABLE_ANNOTATION "
                    "return string-join( ( $settings-array[@TIME_SLOT_ID=$phrase/@TIME_SLOT_REF1]/@TIME_VALUE , $settings-array[@TIME_SLOT_ID=$phrase/@TIME_SLOT_REF2]/@TIME_VALUE ) , ',')" );
    query.evaluateTo(&result);

    if( result.count() != mPhrases.count() )
        return MergeEafWrongNumberOfAnnotations;

    for(int i=0; i<result.count(); i++)
    {
        QStringList tokens = result.at(i).split(",");
        if( tokens.count() != 2)
            continue;
        qlonglong start = tokens.at(0).toLongLong();
        qlonglong end = tokens.at(1).toLongLong();
        mPhrases.at(i)->setAnnotation(Annotation(start, end));
    }

    // read the audio path
    query.setQuery( "string(/ANNOTATION_DOCUMENT/HEADER/MEDIA_DESCRIPTOR/@MEDIA_URL)" );

    QString audioPath;
    query.evaluateTo(&audioPath);

    QUrl theUrl = QUrl::fromEncoded(audioPath.trimmed().toUtf8());

    setSound( theUrl );

    return MergeEafSuccess;
}

QString Text::audioFilePath() const
{
    return mAudioFileURL.toLocalFile();
}

void Text::setAudioFilePath(const QUrl & path)
{
    mAudioFileURL = path;
}

void Text::setBaselineTextForLine( int i, const QString & text )
{
    if( i >= mPhrases.count() )
        return;
    // TODO maybe setLineOfGloss items should just be redone anyway
    setLineOfGlossItems( mPhrases.at(i) , text );
}

QString Text::baselineTextForLine( int i )
{
    if( i >= mPhrases.count() )
        return "";
    return mPhrases.at(i)->equivalentBaselineLineText();
}

Sound* Text::sound()
{
    return mSound;
}

void Text::setSound(const QUrl & filename)
{
    if( mSound != 0 )
        delete mSound;

    mAudioFileURL = filename;

    if( !mAudioFileURL.isEmpty() )
        mSound = new Sound(filename);
}

bool Text::playSoundForLine( int lineNumber )
{
    if( mSound == 0 )
    {
        QMessageBox::warning(0, tr("Error"), tr("This text has no associated sound file.") );
        return false;
    }
    if( !mPhrases.at(lineNumber)->annotation()->isValid() )
    {
        QMessageBox::warning(0, tr("Error"), tr("This phrase does not have a valid annotation (%1, %2).").arg(mPhrases.at(lineNumber)->annotation()->start()).arg(mPhrases.at(lineNumber)->annotation()->end()) );
        return false;
    }

    return mSound->playSegment( mPhrases.at(lineNumber)->annotation()->start() , mPhrases.at(lineNumber)->annotation()->end() );
}

Text::FlexTextReadResult Text::readResult() const
{
    return mReadResult;
}

void Text::setBaselineFromGlossItems()
{
    QStringList phrases;
    foreach(Phrase *phrase, mPhrases)
    {
        QStringList items;
        foreach(GlossItem *item, *phrase->glossItems() )
            items << item->baselineText().text();
        phrases << items.join(" ");
    }
    setBaselineText( phrases.join("\n") );
}

void Text::markAsChanged()
{
    mChanged = true;
}
