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

#include <QtDebug>

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
#include "flextextreader.h"
#include "syntacticanalysis.h"
#include "paragraph.h"

Text::Text(const WritingSystem & ws, const QString & name, Project *project) :
    mSound(nullptr), mReadResult(FlexTextReader::FlexTextReadNoAttempt), mValid(true), mChanged(false), mName(name), mBaselineWritingSystem(ws), mProject(project), mDbAdapter(mProject->dbAdapter())
{
}

Text::Text(const QString & filePath, Project *project) :
    mSound(nullptr), mValid(true), mChanged(false), mName(textNameFromPath(filePath)), mProject(project), mDbAdapter(mProject->dbAdapter())
{
    FlexTextReader reader(this);
    mReadResult = reader.readFile(filePath, true);
    if( mReadResult != FlexTextReader::FlexTextReadSuccess )
        mValid = false;
    mChanged = false;
}

Text::~Text()
{
    qDeleteAll(mParagraphs);
    mParagraphs.clear();

    if( mSound != nullptr )
        delete mSound;
}

QString Text::name() const
{
    return mName;
}

void Text::setName(const QString & name)
{
    mName = name;
    markAsChanged();
}

QString Text::exportFilename() const
{
    if( mExportFilename.isEmpty() )
    {
        return mName + ".flextext";
    }
    else
    {
        return mExportFilename;
    }
}

void Text::setExportFilename(const QString &filename)
{
    mExportFilename = filename;
    markAsChanged();
}

QString Text::comment() const
{
    return mComment;
}

void Text::setComment(const QString & comment)
{
    mComment = comment;
    markAsChanged();
}

bool Text::isChanged() const
{
    return mChanged;
}

WritingSystem Text::baselineWritingSystem() const
{
    return mBaselineWritingSystem;
}

const Project *Text::project() const
{
    return mProject;
}

QList<Paragraph *> *Text::paragraphs()
{
    return &mParagraphs;
}

const QList<Paragraph *> *Text::paragraphs() const
{
    return &mParagraphs;
}

void Text::removeAllParagraphs()
{
    qDeleteAll(mParagraphs);
    mParagraphs.clear();
    markAsChanged();
}

void Text::initializeTextFromString(const QString & content, const QRegularExpression &phraseDelimiter, const QRegularExpression &paragraphDelimiter)
{
    removeAllParagraphs();

    QStringList paragraphs = content.split( paragraphDelimiter );
    foreach(QString paragraph, paragraphs)
    {
        paragraph = paragraph.trimmed();
        mParagraphs.append( new Paragraph(mBaselineWritingSystem) );
        connect( mParagraphs.last(), SIGNAL(changed()), this, SLOT(markAsChanged()) );
        QStringList phrases = paragraph.split( phraseDelimiter );
        foreach(QString phrase, phrases)
        {
            phrase = phrase.trimmed();
            mParagraphs.last()->phrases()->append( new Phrase( this, mProject) );
            mParagraphs.last()->phrases()->last()->connectToText();
            mParagraphs.last()->phrases()->last()->setBaselineText( TextBit( phrase, mBaselineWritingSystem) );
        }
    }

    emit glossItemsChanged();
    markAsChanged();
}

QHash<QString, SyntacticAnalysis *> *Text::syntacticAnalyses()
{
    return &mSyntacticAnalyses;
}

void Text::saveText(bool verboseOutput, bool glossNamespace, bool saveAnyway)
{
    if( mChanged || saveAnyway )
    {
        FlexTextWriter writer( this );
        writer.setVerboseOutput(verboseOutput);
        writer.setIncludeGlossNamespace(glossNamespace);
        writer.writeFile( mProject->filepathFromName(mName) );
        mChanged = false;
    }
}

void Text::writeTextTo(const QString & path, bool verboseOutput, bool glossNamespace)
{
    FlexTextWriter writer( this );
    writer.setVerboseOutput(verboseOutput);
    writer.setIncludeGlossNamespace(glossNamespace);
    writer.writeFile( path );
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
        QMessageBox::critical(nullptr, tr("Error"), tr("The XSL transformation %1 is invalid.").arg(QDir::current().absoluteFilePath("merge-translation-by-line-number.xsl")));
        return XslTranslationError;
    case Xsltproc::InvalidXmlFile:
        QMessageBox::critical(nullptr, tr("Error"), tr("The flextext file %1 is invalid.").arg(currentPath) );
        return XslTranslationError;
    case Xsltproc::CouldNotOpenOutput:
        QMessageBox::critical(nullptr, tr("Error"), tr("The file %1 could not be opened.").arg(tempOutputPath) );
        return XslTranslationError;
    case Xsltproc::ApplyStylesheetFailure:
        if( errorInfo.size() > 0 )
            QDesktopServices::openUrl(QUrl(errorOutputPath, QUrl::TolerantMode));
        else
            QMessageBox::critical(nullptr, tr("Error"), tr("Failure for unknown reasons."));
        QFile::remove(tempOutputPath);
        return XslTranslationError;
    case Xsltproc::GenericFailure:
        QMessageBox::critical(nullptr, tr("Error"), tr("A non-descript error has occurred.") );
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
            markAsChanged();
            return Success;
        }
        else
        {
            return MergeStuckOldFileDeleted;
        }
    }
    else
    {
        return MergeStuckOldFileStillThere;
    }
}

Text::MergeEafResult Text::mergeEaf(const QString & filename )
{
    QStringList result;
    QXmlQuery query(QXmlQuery::XQuery10);
    query.setMessageHandler(new MessageHandler("Text::mergeEaf"));

    query.bindVariable("path", QVariant(QUrl::fromLocalFile(filename).path(QUrl::FullyEncoded)));
    query.setQuery( "declare variable $path external; "
                    "declare variable $settings-array := doc($path)/ANNOTATION_DOCUMENT/TIME_ORDER/TIME_SLOT; "
                    "for $phrase in doc($path)/ANNOTATION_DOCUMENT/TIER/ANNOTATION/ALIGNABLE_ANNOTATION "
                    "return string-join( ( $settings-array[@TIME_SLOT_ID=$phrase/@TIME_SLOT_REF1]/@TIME_VALUE , $settings-array[@TIME_SLOT_ID=$phrase/@TIME_SLOT_REF2]/@TIME_VALUE ) , ',')" );

    query.evaluateTo(&result);

    if( result.count() != phraseCount() )
    {
        qWarning() << result.count() << phraseCount();
        return MergeEafWrongNumberOfAnnotations;
    }

    for(int i=0; i<result.count(); i++)
    {
        QStringList tokens = result.at(i).split(",");
        if( tokens.count() != 2)
            continue;
        qlonglong start = tokens.at(0).toLongLong();
        qlonglong end = tokens.at(1).toLongLong();
        phraseAtLine(i)->setInterval(Interval(start, end));
    }

    // read the audio path
    query.bindVariable("path", QVariant(QUrl::fromLocalFile(filename).path(QUrl::FullyEncoded)));
    query.setQuery( "string(doc($path)/ANNOTATION_DOCUMENT/HEADER/MEDIA_DESCRIPTOR/@MEDIA_URL)" );

    QString audioPath;
    query.evaluateTo(&audioPath);

    QUrl theUrl = QUrl::fromEncoded(audioPath.trimmed().toUtf8());

    setSound( theUrl );

    markAsChanged();
    saveText(false,true,true);

    return MergeEafSuccess;
}

QString Text::textNameFromPath(const QString &path)
{
    QFileInfo info(path);
    return info.baseName();
}

void Text::setSound(const QUrl & filename)
{
    if( mSound != nullptr )
        delete mSound;

    mAudioFileURL = QUrl::fromLocalFile( mProject->mediaPath(filename.toLocalFile()) );

    if( QFileInfo::exists(mAudioFileURL.toLocalFile()) )
        mSound = new Sound(mAudioFileURL);

    markAsChanged();
}

bool Text::playSoundForLine( int lineNumber )
{
    if( mSound == nullptr )
    {
        if( QFileInfo::exists(mAudioFileURL.toLocalFile()) )
        {
            mSound = new Sound(mAudioFileURL);
        }
        else
        {
            QMessageBox::warning(nullptr, tr("Error"), tr("The path to the file does not appear to be valid (%1).").arg(mAudioFileURL.toLocalFile()) );
            return false;
        }
    }
    Phrase * phrase = phraseAtLine(lineNumber);
    if( !phrase->interval()->isValid() )
    {
        QMessageBox::warning(nullptr, tr("Error"), tr("This phrase does not have a valid annotation (%1, %2).").arg(phrase->interval()->start()).arg(phrase->interval()->end()) );
        return false;
    }
    if( mSound != nullptr )
        return mSound->playSegment( phrase->interval()->start() , phrase->interval()->end() );
    else
        return false;
}

FlexTextReader::Result Text::readResult() const
{
    return mReadResult;
}

Paragraph *Text::paragraphForPhrase(Phrase *phrase)
{
    foreach(Paragraph * paragraph, mParagraphs)
    {
        if( paragraph->indexOf(phrase) != -1 )
        {
            return paragraph;
        }
    }
    return nullptr;
}

int Text::lineNumberForPhrase(Phrase *phrase) const
{
    int lineNumber = 0;
    foreach(Paragraph * paragraph, mParagraphs)
    {
        if( paragraph->indexOf(phrase) == -1 )
        {
            lineNumber += paragraph->phraseCount();
        }
        else
        {
            return lineNumber + paragraph->indexOf(phrase);
        }
    }
    return -1;
}

int Text::lineNumberForGlossItem(const GlossItem *item) const
{
    int lineNumber = 0;
    foreach(Paragraph * paragraph, mParagraphs)
    {
        if( paragraph->indexOfGlossItem( item ) == -1 )
        {
            lineNumber += paragraph->phraseCount();
        }
        else
        {
            return lineNumber + paragraph->indexOfGlossItem( item );
        }
    }
    return -1;
}

void Text::removeParagraphDivision(int paragraphIndex)
{
    Q_ASSERT( paragraphIndex < mParagraphs.count() );
    if( paragraphIndex > 0 )
    {
        Paragraph * first = mParagraphs.at(paragraphIndex-1);
        Paragraph * second = mParagraphs.at(paragraphIndex);
        for(int i=0; i<second->phraseCount(); i++)
        {
            first->phrases()->append( second->phrases()->takeAt(0) );
        }
        delete mParagraphs.takeAt(paragraphIndex);
        emit guiRefreshRequest();
        markAsChanged();
    }
}

void Text::addParagraphDivision(int lineNumber)
{
    if( lineNumber == 0 )
        return;
    Phrase * phrase = phraseAtLine(lineNumber);
    Q_ASSERT(phrase != nullptr);
    Paragraph * paragraph = paragraphForPhrase(phrase);
    Q_ASSERT(paragraph != nullptr);
    int paragraphIndex = mParagraphs.indexOf(paragraph);
    int phraseIndex = paragraph->indexOf(phrase);
    Q_ASSERT(paragraphIndex != -1);
    Q_ASSERT(phraseIndex != -1);
    addParagraphDivision(paragraphIndex, phraseIndex);
}

void Text::addParagraphDivision(int paragraphIndex, int phraseIndex)
{
    Q_ASSERT( paragraphIndex < mParagraphs.count() );
    Q_ASSERT( phraseIndex < mParagraphs.at(paragraphIndex)->phraseCount() );
    Paragraph * newParagraph = new Paragraph(mBaselineWritingSystem);
    connect( newParagraph, SIGNAL(changed()), this, SLOT(markAsChanged()) );
    for(int i=phraseIndex; i<mParagraphs.at(paragraphIndex)->phraseCount(); i++)
    {
        newParagraph->phrases()->append( mParagraphs.at(paragraphIndex)->phrases()->takeAt(phraseIndex) );
    }
    mParagraphs.insert(paragraphIndex+1,newParagraph);
    emit requestGuiRefresh(newParagraph->phrases()->first());
    markAsChanged();
}

void Text::removeParagraphDivision(Paragraph *paragraph)
{
    int index = mParagraphs.indexOf(paragraph);
    if( index != -1 )
    {
        removeParagraphDivision(index);
    }
}

Phrase *Text::phraseAtLine(int lineNumber)
{
    int lineCount = 0;
    foreach(Paragraph * paragraph, mParagraphs)
    {
        if( lineNumber < lineCount + paragraph->phraseCount() )
        {
            return paragraph->phrases()->at( lineNumber - lineCount );
        }
        else
        {
            lineCount += paragraph->phraseCount();
        }
    }
    return nullptr;
}

const Phrase *Text::phraseAtLine(int lineNumber) const
{
    int lineCount = 0;
    foreach(Paragraph * paragraph, mParagraphs)
    {
        if( lineNumber < lineCount + paragraph->phraseCount() )
        {
            return paragraph->phrases()->at( lineNumber - lineCount );
        }
        else
        {
            lineCount += paragraph->phraseCount();
        }
    }
    return nullptr;
}

int Text::phraseCount() const
{
    int lineCount = 0;
    foreach(const Paragraph * paragraph, mParagraphs)
    {
        lineCount += paragraph->phraseCount();
    }
    return lineCount;
}

void Text::markAsChanged()
{
    mChanged = true;
}

void Text::requestGuiRefresh( Phrase * phrase )
{
    int lineNumber = lineNumberForPhrase(phrase);
    if( lineNumber != -1 )
        emit phraseRefreshNeeded( lineNumber );
}

void Text::removePhrase(int lineNumber )
{
    foreach(Paragraph * paragraph, mParagraphs)
    {
        if( paragraph->removePhrase( phraseAtLine(lineNumber) ) )
        {
            if( paragraph->phraseCount() == 0 )
            {
                removeParagraphDivision( mParagraphs.indexOf(paragraph) );
            }
            markAsChanged();
            if( lineNumber < phraseCount() )
            {
                emit phraseRefreshNeeded( lineNumber );
            }
            return;
        }
    }
}

void Text::mergePhraseWithPrevious(int lineNumber)
{
    if( lineNumber == 0 || lineNumber >= phraseCount() )
    {
        return;
    }
    Phrase * earlierPhrase = phraseAtLine(lineNumber-1);
    Phrase * latterPhrase = phraseAtLine(lineNumber);

    int nItems = latterPhrase->glossItemCount();
    for(int i=0; i< nItems; i++)
    {
        earlierPhrase->appendGlossItem( latterPhrase->takeGlossItemAt(0) );
    }
    removePhrase(lineNumber);
    emit guiRefreshRequest();
    markAsChanged();
}

void Text::findGlossItemLocation(const GlossItem *glossItem, int & paragraph, int & phrase, int & position) const
{
    paragraph = -1;
    phrase = -1;
    position = 0;
    for(int i=0; i<mParagraphs.count(); i++)
    {
        for(int j=0; j<mParagraphs.at(i)->phraseCount(); j++)
        {
            int index = mParagraphs.at(i)->phrases()->at(j)->indexOfGlossItem(glossItem);
            if( index != -1 )
            {
                paragraph = i;
                phrase = j;
                position = index;
                return;
            }
            else
            {
                position += mParagraphs.at(i)->phraseCount();
            }
        }
    }
    // interesting that this is the preferred strategy!
    // http://stackoverflow.com/questions/1257744/can-i-use-break-to-exit-multiple-nested-for-loops
    if( phrase == -1 )
        position = -1;
}

void Text::setFollowingInterpretations( GlossItem *glossItem )
{
    QString textForm = glossItem->baselineText().text();
    qlonglong interpretationId = glossItem->id();

    int startingParagraph, startingPhrase, startingGlossItem;
    findGlossItemLocation(glossItem, startingParagraph, startingPhrase, startingGlossItem );
    if( startingPhrase == -1 || startingGlossItem == -1 )
        return;

    // do the remainder of the starting paragraph and phrase
    for(int i=startingGlossItem+1; i < mParagraphs.at(startingParagraph)->phrases()->at(startingPhrase)->glossItemCount(); i++ )
    {
        if( mParagraphs.at(startingParagraph)->phrases()->at(startingPhrase)->glossItemAt(i)->baselineText().text() == textForm )
        {
            mParagraphs.at(startingParagraph)->phrases()->at(startingPhrase)->glossItemAt(i)->setInterpretation( interpretationId );
        }
    }

    for(int i=startingParagraph+1; i<mParagraphs.count(); i++)
    {
        for(int j=0; j < mParagraphs.at(i)->phraseCount(); j++ )
        {
            for(int k=0; k < mParagraphs.at(i)->phrases()->at(j)->glossItemCount(); k++ )
            {
                if( mParagraphs.at(i)->phrases()->at(j)->glossItemAt(k)->baselineText().text() == textForm )
                {
                    mParagraphs.at(i)->phrases()->at(j)->glossItemAt(k)->setInterpretation( interpretationId );
                }
            }
        }
    }

    markAsChanged();
}

void Text::replaceFollowing( GlossItem *glossItem, const QString & searchFor )
{
    int startingParagraph, startingPhrase, startingGlossItem;
    findGlossItemLocation(glossItem, startingParagraph, startingPhrase, startingGlossItem );
    if( startingPhrase == -1 || startingGlossItem == -1 )
        return;

    // @todo this looks like a copy/paste error ... it's set interpretation where it should be a replace operation
    // you need to check the interface to see what is supposed to happen

    // do the remainder of the starting paragraph and phrase
    for(int i=startingGlossItem+1; i < mParagraphs.at(startingParagraph)->phrases()->at(startingPhrase)->glossItemCount(); i++ )
    {
        if( mParagraphs.at(startingParagraph)->phrases()->at(startingPhrase)->glossItemAt(i)->baselineText().text() == searchFor )
        {
            mParagraphs.at(startingParagraph)->phrases()->at(startingPhrase)->glossItemAt(i)->setInterpretation( glossItem->id() );
        }
    }

    for(int i=startingParagraph+1; i<mParagraphs.count(); i++)
    {
        for(int j=0; j < mParagraphs.at(i)->phraseCount(); j++ )
        {
            for(int k=0; k < mParagraphs.at(i)->phrases()->at(j)->glossItemCount(); k++ )
            {
                if( mParagraphs.at(i)->phrases()->at(j)->glossItemAt(k)->baselineText().text() == searchFor )
                {
                    mParagraphs.at(i)->phrases()->at(j)->glossItemAt(k)->setInterpretation( glossItem->id() );
                }
            }
        }
    }

    markAsChanged();
}

void Text::matchFollowingTextForms(GlossItem *glossItem, const WritingSystem & ws )
{
    int startingParagraph, startingPhrase, startingGlossItem;
    findGlossItemLocation(glossItem, startingParagraph, startingPhrase, startingGlossItem );
    if( startingPhrase == -1 || startingGlossItem == -1 )
        return;

    // do the remainder of the starting phrase
    for(int i=startingGlossItem+1; i < mParagraphs.at(startingParagraph)->phrases()->at(startingPhrase)->glossItemCount(); i++ )
    {
        if( mParagraphs.at(startingParagraph)->phrases()->at(startingPhrase)->glossItemAt(i)->id() == glossItem->id() )
        {
            mParagraphs.at(startingParagraph)->phrases()->at(startingPhrase)->glossItemAt(i)->setTextForm( glossItem->textForm(ws) );
        }
    }

    for(int i=startingParagraph+1; i<mParagraphs.count(); i++)
    {
        for(int j=0; j < mParagraphs.at(i)->phraseCount(); j++ )
        {
            for(int k=0; k < mParagraphs.at(i)->phrases()->at(j)->glossItemCount(); k++ )
            {
                if( mParagraphs.at(i)->phrases()->at(j)->glossItemAt(k)->id() == glossItem->id() )
                {
                    mParagraphs.at(i)->phrases()->at(j)->glossItemAt(k)->setTextForm( glossItem->textForm(ws) );
                }
            }
        }
    }

    markAsChanged();
}

void Text::matchFollowingGlosses(GlossItem *glossItem, const WritingSystem & ws )
{
    int startingParagraph, startingPhrase, startingGlossItem;
    findGlossItemLocation(glossItem, startingParagraph, startingPhrase, startingGlossItem );
    if( startingPhrase == -1 || startingGlossItem == -1 )
        return;

    // do the remainder of the starting phrase
    for(int i=startingGlossItem+1; i < mParagraphs.at(startingParagraph)->phrases()->at(startingPhrase)->glossItemCount(); i++ )
    {
        if( mParagraphs.at(startingParagraph)->phrases()->at(startingPhrase)->glossItemAt(i)->id() == glossItem->id() )
        {
            mParagraphs.at(startingParagraph)->phrases()->at(startingPhrase)->glossItemAt(i)->setGloss( glossItem->gloss(ws) );
        }
    }

    for(int i=startingParagraph+1; i<mParagraphs.count(); i++)
    {
        for(int j=0; j < mParagraphs.at(i)->phraseCount(); j++ )
        {
            for(int k=0; k < mParagraphs.at(i)->phrases()->at(j)->glossItemCount(); k++ )
            {
                if( mParagraphs.at(i)->phrases()->at(j)->glossItemAt(k)->id() == glossItem->id() )
                {
                    mParagraphs.at(i)->phrases()->at(j)->glossItemAt(k)->setGloss( glossItem->gloss(ws) );
                }
            }
        }
    }

    markAsChanged();
}

void Text::newPhraseStartingHere(GlossItem *glossItem)
{
    int startingParagraph, startingPhrase, startingGlossItem;
    findGlossItemLocation(glossItem, startingParagraph, startingPhrase, startingGlossItem );

    Phrase * newPhrase = new Phrase( this, mProject);
    newPhrase->connectToText();

    Phrase * sourcePhrase = mParagraphs.at(startingParagraph)->phrases()->at(startingPhrase);
    int mx = sourcePhrase->glossItemCount() - startingGlossItem;
    for(int i=0; i<mx; i++)
    {
        newPhrase->appendGlossItem( sourcePhrase->takeGlossItemAt(startingGlossItem) );
    }

    mParagraphs.at(startingParagraph)->insertPhrase( startingPhrase + 1, newPhrase);

    emit guiRefreshRequest();
    markAsChanged();
}

void Text::noNewPhraseStartingHere(GlossItem *glossItem)
{
    int paragraphIndex, phraseIndex, glossItemIndex;
    findGlossItemLocation(glossItem, paragraphIndex, phraseIndex, glossItemIndex );

    if( phraseIndex < 1 ) return;

    Phrase * first = mParagraphs.at(paragraphIndex)->phrases()->at(phraseIndex-1);
    Phrase * second = mParagraphs.at(paragraphIndex)->phrases()->at(phraseIndex);

    while( second->glossItemCount() > 0 )
    {
        first->appendGlossItem( second->takeGlossItemAt(0) );
    }

    removePhrase(second); /// this will then emit the signals necessary for a refresh, etc.
    markAsChanged();
}

void Text::baselineSearchReplace( const TextBit & search , const TextBit & replace )
{
    for(int i=0; i < mParagraphs.count(); i++ )
    {
        for(int j=0; j < mParagraphs.at(i)->phraseCount(); j++ )
        {
            for(int k=0; k < mParagraphs.at(i)->phrases()->at(j)->glossItemCount(); k++ )
            {
                if( mParagraphs.at(i)->phrases()->at(j)->glossItemAt(k)->baselineText() == search )
                {
                    mParagraphs.at(i)->phrases()->at(j)->glossItemAt(k)->resetBaselineText( replace );
                }
            }
        }
    }
    markAsChanged();
}

void Text::registerPhrasalGlossChange(Phrase * thisPhrase, const TextBit & bit)
{
    int lineNumber = lineNumberForPhrase(thisPhrase);
    if( lineNumber != -1 )
    {
        emit phrasalGlossChanged(lineNumber, bit);
        markAsChanged();
    }
}

void Text::removePhrase( Phrase * phrase )
{
    foreach(Paragraph * paragraph, mParagraphs)
    {
        paragraph->removePhrase(phrase);
    }
    emit guiRefreshRequest();
    markAsChanged();
}
