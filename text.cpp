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
#include "flextextwriter.h"

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
            mPhrases.append( new Phrase( this, mProject) );
            connect( mPhrases.last(), SIGNAL(phraseChanged()), this, SLOT(setBaselineFromGlossItems()) );
            connect( mPhrases.last(), SIGNAL(requestGuiRefresh(Phrase*)), this, SLOT(requestGuiRefresh(Phrase*)));
            connect( mPhrases.last(), SIGNAL(glossChanged()), this, SLOT(markAsChanged()));

            setLineOfGlossItems(mPhrases.last(), lines.at(i));
            if( progress.wasCanceled() )
            {
                mValid = false;
                break;
            }
        }
        progress.setValue(lines.count());
    }
    emit glossItemsChanged();
}

void Text::setLineOfGlossItems( Phrase * phrase , const QString & line )
{
    phrase->clearGlossItems();

    QStringList words = line.split(QRegExp("[ \\t]+"),QString::SkipEmptyParts);
    for(int i=0; i<words.count(); i++)
        phrase->appendGlossItem(new GlossItem(TextBit(words.at(i),mBaselineWritingSystem), mProject ));

    emit phraseRefreshNeeded( mPhrases.indexOf(phrase) );
}

bool Text::setBaselineWritingSystemFromFile(const QString & filePath )
{
    QXmlQuery query(QXmlQuery::XQuery10);
    query.setMessageHandler(new MessageHandler("Text::setBaselineWritingSystemFromFile", this));
    query.bindVariable("path", QVariant(QUrl(filePath).path()));
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
    QList<MorphologicalAnalysis*> morphologicalAnalyses;
    MorphologicalAnalysis *morphologicalAnalysis = 0;
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
                mPhrases.append( new Phrase(this, mProject) );
                connect( mPhrases.last(), SIGNAL(phraseChanged()), this, SLOT(setBaselineFromGlossItems()) );
                connect( mPhrases.last(), SIGNAL(requestGuiRefresh(Phrase*)), this, SLOT(requestGuiRefresh(Phrase*)));
                connect( mPhrases.last(), SIGNAL(glossChanged()), this, SLOT(markAsChanged()));

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
                if( attr.hasAttribute("type") )
                {
                    QString type = attr.value("type").toString();
                    WritingSystem lang;
                    if( attr.hasAttribute("lang") )
                            lang = mProject->dbAdapter()->writingSystem( attr.value("lang").toString() );
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
                        if( type == "txt" )
                        {
                            TextBit textForm( text , lang, itemId);
                            textForms.insert( lang, textForm );
                            if( lang.flexString() == mBaselineWritingSystem.flexString() )
                                baselineText = text;
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
                    WritingSystem ws = mDbAdapter->writingSystem( attr.value("http://www.adambaker.org/gloss.php","lang").toString() );
                    TextBit corresponding = textForms.value( ws, TextBit() );
                    if( corresponding.isValid() )
                        morphologicalAnalysis = new MorphologicalAnalysis(corresponding);
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
                }

                mPhrases.last()->appendGlossItem(new GlossItem( mBaselineWritingSystem, textForms, glossForms, id, mProject ));
                mPhrases.last()->lastGlossItem()->setApprovalStatus(approvalStatus);

                QListIterator<MorphologicalAnalysis*> iter(morphologicalAnalyses);
                while(iter.hasNext())
                    mPhrases.last()->lastGlossItem()->setMorphologicalAnalysis( *iter.next() );
                qDeleteAll( morphologicalAnalyses );
                morphologicalAnalyses.clear();

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
                if( !morphologicalAnalysis->isEmpty() )
                    morphologicalAnalyses << morphologicalAnalysis;
                morphologicalAnalysis = 0;
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

void Text::saveText(bool verboseOutput, bool saveAnyway)
{
    if( mChanged || saveAnyway )
    {
        FlexTextWriter writer( this, verboseOutput );
        writer.writeFile( mProject->filepathFromName(mName) );
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
    query.setMessageHandler(new MessageHandler("Text::mergeEaf"));
    query.bindVariable("path", QVariant(QUrl(filename).path()));
    query.setQuery( "declare variable $path external; "
                    "declare variable $settings-array := doc($path)/ANNOTATION_DOCUMENT/TIME_ORDER/TIME_SLOT; "
                    "for $phrase in doc($path)/ANNOTATION_DOCUMENT/TIER/ANNOTATION/ALIGNABLE_ANNOTATION "
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

    markAsChanged();

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

void Text::requestGuiRefresh( Phrase * phrase )
{
    int lineNumber = mPhrases.indexOf(phrase);
    if( lineNumber != -1 )
        emit phraseRefreshNeeded( lineNumber );
}

void Text::removeLine( int lineNumber )
{
    if( lineNumber < mPhrases.count() )
    {
        markAsChanged();

        QStringList lines = mBaselineText.split(QRegExp("[\\n\\r]+"),QString::SkipEmptyParts);
        lines.removeAt(lineNumber);
        mBaselineText = lines.join( "\n" );

        delete mPhrases.takeAt( lineNumber );
        emit baselineTextChanged(mBaselineText);
        emit glossItemsChanged();
    }
}

void Text::findGlossItemLocation(GlossItem *glossItem, int & line, int & position)
{
    line = -1;
    position = -1;
    for(int i=0; i<mPhrases.count(); i++)
    {
        int index = mPhrases.at(i)->indexOfGlossItem(glossItem);
        if( index != -1 )
        {
            line = i;
            position = index;
        }
    }
}

void Text::setFollowingInterpretations( GlossItem *glossItem )
{
    QString textForm = glossItem->baselineText().text();
    qlonglong interpretationId = glossItem->id();

    int startingPhrase, startingGlossItem;
    findGlossItemLocation(glossItem, startingPhrase, startingGlossItem );
    if( startingPhrase == -1 || startingGlossItem == -1 )
        return;

    // do the remainder of the starting phrase
    for(int i=startingGlossItem+1; i < mPhrases.at(startingPhrase)->glossItemCount(); i++ )
    {
        if( mPhrases.at(startingPhrase)->glossItemAt(i)->baselineText().text() == textForm )
        {
            mPhrases.at(startingPhrase)->glossItemAt(i)->setInterpretation( interpretationId );
        }
    }

    for(int i=startingPhrase+1; i < mPhrases.count(); i++ )
    {
        for(int j=0; j < mPhrases.at(i)->glossItemCount(); j++ )
        {
            if( mPhrases.at(i)->glossItemAt(j)->baselineText().text() == textForm )
            {
                mPhrases.at(i)->glossItemAt(j)->setInterpretation( interpretationId );
            }
        }
    }
}

void Text::replaceFollowing( GlossItem *glossItem, const QString & searchFor )
{
    int startingPhrase, startingGlossItem;
    findGlossItemLocation(glossItem, startingPhrase, startingGlossItem );
    if( startingPhrase == -1 || startingGlossItem == -1 )
        return;

    // do the remainder of the starting phrase
    for(int i=startingGlossItem+1; i < mPhrases.at(startingPhrase)->glossItemCount(); i++ )
    {
        if( mPhrases.at(startingPhrase)->glossItemAt(i)->baselineText().text() == searchFor )
        {
            mPhrases.at(startingPhrase)->glossItemAt(i)->setInterpretation( glossItem->id() );
        }
    }

    for(int i=startingPhrase+1; i < mPhrases.count(); i++ )
    {
        for(int j=0; j < mPhrases.at(i)->glossItemCount(); j++ )
        {
            if( mPhrases.at(i)->glossItemAt(j)->baselineText().text() == searchFor )
            {
                mPhrases.at(i)->glossItemAt(j)->setInterpretation( glossItem->id() );
            }
        }
    }
}
