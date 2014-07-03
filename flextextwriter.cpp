#include "flextextwriter.h"

#include "text.h"
#include "phrase.h"
#include "glossitem.h"
#include "allomorph.h"
#include "databaseadapter.h"
#include "project.h"

#include <QFile>
#include <QXmlStreamWriter>
#include <QProgressDialog>
#include <QtDebug>

FlexTextWriter::FlexTextWriter(Text *text) : mText(text)
{
    mVerboseOutput = false;
    mIncludeGlossNamespace = true;
    mIncludeMorphologicalAnalysis = false;
}

bool FlexTextWriter::writeFile( const QString & filename )
{
    QFile outFile(filename);
    if( !outFile.open(QFile::WriteOnly | QFile::Text) )
        return false;

    stream = new QXmlStreamWriter(&outFile);
    stream->setCodec("UTF-8");
    stream->setAutoFormatting(true);
    if( mIncludeGlossNamespace )
    {
        stream->writeNamespace("http://www.adambaker.org/gloss.php", "abg" );
    }

    stream->writeStartDocument();
    stream->writeStartElement("document");
    stream->writeAttribute("version", "2");

    serializeInterlinearText();

    stream->writeEndElement(); // document
    stream->writeEndDocument();

    delete stream;

    return true;
}

bool FlexTextWriter::serializeInterlinearText() const
{
    stream->writeStartElement("interlinear-text");

    writeNamespaceAttribute( "baseline-writing-system", mText->project()->metaLanguage().flexString() );

    if( !mText->mAudioFileURL.isEmpty() )
        writeNamespaceAttribute( "audio-file", mText->mAudioFileURL.toString() );

    if( !mText->mName.isEmpty() )
        serializeItem( "title" , mText->project()->metaLanguage() , mText->mName  );
    if( !mText->mComment.isEmpty() )
        serializeItem( "comment" , mText->project()->metaLanguage() , mText->mComment  );

    stream->writeStartElement("paragraphs");

    QProgressDialog progress( QObject::tr("Saving text %1...").arg(mText->mName), QString(), 0, mText->mPhrases.count(), 0);
    progress.setWindowModality(Qt::WindowModal);

    int count = 0;
    foreach( Phrase* phrase, mText->mPhrases )
    {
        count++;

        progress.setValue(count);

        // this rather profligate nesting seems to be a feature of flextext files
        stream->writeStartElement("paragraph");
        stream->writeStartElement("phrases");
        stream->writeStartElement("phrase");

        if( !phrase->interval()->isNull() )
        {
            writeNamespaceAttribute( "annotation-start", QString("%1").arg(phrase->interval()->start()) );
            writeNamespaceAttribute( "annotation-end", QString("%1").arg(phrase->interval()->end()) );
        }

        serializeItem("segnum", mText->project()->metaLanguage(), QString("%1").arg(count)  );

        stream->writeStartElement("words");
        for(int i=0; i<phrase->glossItemCount(); i++ )
        {
            GlossItem *glossItem = phrase->glossItemAt(i);

            if( glossItem->isPunctuation() )
            {
                serializePunctuation(glossItem);
            }
            else
            {
                serializeGlossItem(glossItem);
            }
        }

        stream->writeEndElement(); // words

        // phrase-level glosses
        TextBitHashIterator iter = phrase->glosses();
        while (iter.hasNext())
        {
            iter.next();
            serializeItem("gls",iter.key(),iter.value().text());
        }

        stream->writeEndElement(); // phrase
        stream->writeEndElement(); // phrases
        stream->writeEndElement(); // paragraph
    }

    progress.setValue(mText->mPhrases.count());

    stream->writeEndElement(); // paragraphs

    serializeLanguages();

    stream->writeEndElement(); // interlinear-text

    return true;
}

bool FlexTextWriter::serializeGlossItem(GlossItem *glossItem) const
{
    stream->writeStartElement("word");

    writeNamespaceAttribute( "id", QString("%1").arg(glossItem->id()) );
    if( glossItem->approvalStatus() == GlossItem::Approved )
        writeNamespaceAttribute( "approval-status", "true" );
    else
        writeNamespaceAttribute( "approval-status", "false" );

    if( glossItem->baselineWritingSystem() != mText->baselineWritingSystem() )
    {
        writeNamespaceAttribute( "baseline-writing-system", glossItem->baselineWritingSystem().flexString() );
    }

    TextBitHashIterator textIter(*glossItem->textForms());

    while (textIter.hasNext())
    {
        textIter.next();
        if( mVerboseOutput )
            serializeItem("txt",textIter.key(),textIter.value().text() , textIter.value().id() );
        else
            serializeItemNonVerbose("txt", textIter.key() , textIter.value().id() );
    }

    TextBitHashIterator glossIter(*glossItem->glosses());

    while (glossIter.hasNext())
    {
        glossIter.next();
        if( mVerboseOutput )
            serializeItem("gls",glossIter.key(),glossIter.value().text(), glossIter.value().id());
        else
            serializeItemNonVerbose("gls", glossIter.key() , glossIter.value().id() );
    }

    if( mIncludeMorphologicalAnalysis )
    {
        serializeMorphemes( glossItem );
    }

    // custom annotations
    if( mIncludeGlossNamespace )
    {
        QHashIterator<QString,TextBit> annIter = glossItem->annotations();
        while ( annIter.hasNext() )
        {
            annIter.next();

            stream->writeStartElement("http://www.adambaker.org/gloss.php", "annotation");
            stream->writeAttribute("key", annIter.key() );
            stream->writeAttribute("lang", annIter.value().writingSystem().flexString() );
            stream->writeCharacters(annIter.value().text());
            stream->writeEndElement();
        }
    }
    stream->writeEndElement(); // word

    return true;
}

bool FlexTextWriter::serializePunctuation(GlossItem *glossItem) const
{
    stream->writeStartElement("word");
    writeNamespaceAttribute( "id", QString("%1").arg(glossItem->id()) );
    serializeItem("punct", glossItem->baselineWritingSystem(), glossItem->baselineText().text(), glossItem->baselineText().id() );
    stream->writeEndElement(); // word
    return true;
}

bool FlexTextWriter::serializeMorphemes(GlossItem *glossItem) const
{
    glossItem->loadMorphologicalAnalysesFromDatabase();
    QList<WritingSystem> analysisLanguages = glossItem->morphologicalAnalysisLanguages();
    foreach( WritingSystem ws, analysisLanguages )
    {
        const MorphologicalAnalysis * analysis = glossItem->morphologicalAnalysis( ws );
        if( ! analysis->isEmpty() )
        {
            stream->writeStartElement("morphemes");
            writeNamespaceAttribute( "lang", ws.flexString() );

            AllomorphIterator iter = analysis->allomorphIterator();
            while(iter.hasNext())
                serializeAllomorph( iter.next()  );
            stream->writeEndElement(); // morphemes
        }
    }
    return true;
}

bool FlexTextWriter::serializeAllomorph(const Allomorph & allomorph) const
{
    stream->writeStartElement("morph");

    stream->writeAttribute("type", allomorph.typeString());
    writeNamespaceAttribute( "id", QString("%1").arg(allomorph.id()) );

    serializeItem( "txt" , allomorph.writingSystem(), allomorph.text() );

    TextBitHash citationForms = mText->mDbAdapter->lexicalEntryCitationFormsForAllomorph( allomorph.id() );
    TextBitHashIterator citationIter(citationForms);
    while( citationIter.hasNext() )
    {
        citationIter.next();
        serializeItem( "cf", citationIter.key(), citationIter.value().text(), citationIter.value().id() );
    }

    TextBitHash glossForms = mText->mDbAdapter->lexicalEntryGlossFormsForAllomorph( allomorph.id() );
    TextBitHashIterator glossIter(glossForms);
    while( glossIter.hasNext() )
    {
        glossIter.next();
        serializeItem( "gls", glossIter.key(), glossIter.value().text(), glossIter.value().id() );
    }

    if( mIncludeGlossNamespace )
    {
        QStringListIterator tagIter( mText->mDbAdapter->grammaticalTagsForAllomorph( allomorph.id() ) );
        while( tagIter.hasNext() )
        {
            stream->writeEmptyElement("http://www.adambaker.org/gloss.php", "grammatical-tag");
            stream->writeAttribute("http://www.adambaker.org/gloss.php", "name", tagIter.next() );
        }
    }

    stream->writeEndElement(); // morph

    return true;
}

bool FlexTextWriter::serializeLanguages() const
{
    stream->writeStartElement("languages");
    QList<WritingSystem> ws = mText->mDbAdapter->writingSystems();
    for( int i=0; i<ws.count(); i++ )
    {
        stream->writeStartElement("language");
        stream->writeAttribute("lang", ws.at(i).flexString() );
        stream->writeAttribute("font", ws.at(i).fontFamily() );
        writeNamespaceAttribute("font-size", QString("%1").arg(ws.at(i).fontSize()) );
        if( ws.at(i).layoutDirection() == Qt::RightToLeft )
            stream->writeAttribute("RightToLeft", "true" );
        if( mText->mBaselineWritingSystem.flexString() == ws.at(i).flexString() )
            writeNamespaceAttribute("is-baseline", "true" );

        stream->writeEndElement(); // language
    }
    stream->writeEndElement(); // languages

    return true;
}

void FlexTextWriter::serializeItem(const QString & type, const WritingSystem & ws, const QString & text , qlonglong id) const
{
    stream->writeStartElement("item");
    if( id != -1 )
        writeNamespaceAttribute( "id", QString("%1").arg(id) );
    stream->writeAttribute("type",type);
    stream->writeAttribute("lang",ws.flexString());
    stream->writeCharacters(text);
    stream->writeEndElement();
}

void FlexTextWriter::serializeItemNonVerbose(const QString & type, const WritingSystem & ws, qlonglong id ) const
{
    stream->writeStartElement("item");
    if( id != -1 )
        writeNamespaceAttribute( "id", QString("%1").arg(id) );
    stream->writeAttribute("type",type);
    stream->writeAttribute("lang",ws.flexString());
    stream->writeEndElement();
}

void FlexTextWriter::setVerboseOutput(bool value)
{
    mVerboseOutput = value;
}

void FlexTextWriter::setIncludeGlossNamespace(bool value)
{
    mIncludeGlossNamespace = value;
}

void FlexTextWriter::setIncludeMorphologicalAnalysis(bool value)
{
    mIncludeMorphologicalAnalysis = value;
}

void FlexTextWriter::writeNamespaceAttribute(const QString &name, const QString &value) const
{
    if( mIncludeGlossNamespace )
    {
        stream->writeAttribute("http://www.adambaker.org/gloss.php", name, value );
    }
}

