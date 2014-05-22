#include "flextextwriter.h"

#include "text.h"
#include "phrase.h"
#include "glossitem.h"
#include "allomorph.h"
#include "databaseadapter.h"

#include <QFile>
#include <QXmlStreamWriter>
#include <QProgressDialog>
#include <QtDebug>

FlexTextWriter::FlexTextWriter(Text *text, bool verboseOutput)
{
    mText = text;
    mVerboseOutput = verboseOutput;
}

bool FlexTextWriter::writeFile( const QString & filename )
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

bool FlexTextWriter::serializeInterlinearText(QXmlStreamWriter *stream) const
{
    stream->writeStartElement("interlinear-text");

    stream->writeAttribute("http://www.adambaker.org/gloss.php","baseline-writing-system", mText->mDbAdapter->metaLanguage().flexString() );

    if( !mText->mAudioFileURL.isEmpty() )
        stream->writeAttribute("http://www.adambaker.org/gloss.php","audio-file", mText->mAudioFileURL.toString() );

    if( !mText->mName.isEmpty() )
        serializeItem( "title" , mText->mDbAdapter->metaLanguage() , mText->mName , stream );
    if( !mText->mComment.isEmpty() )
        serializeItem( "comment" , mText->mDbAdapter->metaLanguage() , mText->mComment , stream );

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
            stream->writeAttribute("http://www.adambaker.org/gloss.php","annotation-start", QString("%1").arg(phrase->interval()->start()) );
            stream->writeAttribute("http://www.adambaker.org/gloss.php","annotation-end", QString("%1").arg(phrase->interval()->end()) );
        }

        serializeItem("segnum", mText->mDbAdapter->metaLanguage(), QString("%1").arg(count) , stream );

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

    progress.setValue(mText->mPhrases.count());

    stream->writeEndElement(); // paragraphs

    serializeLanguages(stream);

    stream->writeEndElement(); // interlinear-text

    return true;
}

bool FlexTextWriter::serializeGlossItem(GlossItem *glossItem, QXmlStreamWriter *stream) const
{
    stream->writeStartElement("word");

    stream->writeAttribute("http://www.adambaker.org/gloss.php", "id", QString("%1").arg(glossItem->id()) );
    if( glossItem->approvalStatus() == GlossItem::Approved )
        stream->writeAttribute("http://www.adambaker.org/gloss.php", "approval-status", "true" );
    else
        stream->writeAttribute("http://www.adambaker.org/gloss.php", "approval-status", "false" );

    if( glossItem->baselineWritingSystem() != mText->baselineWritingSystem() )
    {
        stream->writeAttribute("http://www.adambaker.org/gloss.php", "baseline-writing-system", glossItem->baselineWritingSystem().flexString() );
    }

    TextBitHashIterator textIter(*glossItem->textForms());

    while (textIter.hasNext())
    {
        textIter.next();
        if( mVerboseOutput )
            serializeItem("txt",textIter.key(),textIter.value().text() ,stream , textIter.value().id() );
        else
            serializeItemNonVerbose("txt", textIter.key(), stream , textIter.value().id() );
    }

    TextBitHashIterator glossIter(*glossItem->glosses());

    while (glossIter.hasNext())
    {
        glossIter.next();
        if( mVerboseOutput )
            serializeItem("gls",glossIter.key(),glossIter.value().text(),stream, glossIter.value().id());
        else
            serializeItemNonVerbose("gls", glossIter.key(), stream , glossIter.value().id() );
    }

    if( mVerboseOutput )
        serializeMorphemes( glossItem, stream );

    // custom annotations
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

    stream->writeEndElement(); // word

    return true;
}

bool FlexTextWriter::serializeMorphemes(GlossItem *glossItem, QXmlStreamWriter *stream) const
{
    QList<WritingSystem> analysisLanguages = glossItem->morphologicalAnalysisLanguages();
    foreach( WritingSystem ws, analysisLanguages )
    {
        const MorphologicalAnalysis * analysis = glossItem->morphologicalAnalysis( ws );
        if( ! analysis->isEmpty() )
        {
            stream->writeStartElement("morphemes");
            stream->writeAttribute("http://www.adambaker.org/gloss.php", "lang", ws.flexString() );

            AllomorphIterator iter = analysis->allomorphIterator();
            while(iter.hasNext())
                serializeAllomorph( iter.next() , stream );
            stream->writeEndElement(); // morphemes
        }
    }
    return true;
}

bool FlexTextWriter::serializeAllomorph(const Allomorph & allomorph, QXmlStreamWriter *stream) const
{
    stream->writeStartElement("morph");

    stream->writeAttribute("type", allomorph.typeString());
    stream->writeAttribute("http://www.adambaker.org/gloss.php", "id", QString("%1").arg(allomorph.id()) );

    serializeItem( "txt" , allomorph.writingSystem(), allomorph.text(), stream );

    TextBitHash citationForms = mText->mDbAdapter->lexicalEntryCitationFormsForAllomorph( allomorph.id() );
    TextBitHashIterator citationIter(citationForms);
    while( citationIter.hasNext() )
    {
        citationIter.next();
        serializeItem( "cf", citationIter.key(), citationIter.value().text(), stream, citationIter.value().id() );
    }

    TextBitHash glossForms = mText->mDbAdapter->lexicalEntryGlossFormsForAllomorph( allomorph.id() );
    TextBitHashIterator glossIter(glossForms);
    while( glossIter.hasNext() )
    {
        glossIter.next();
        serializeItem( "gls", glossIter.key(), glossIter.value().text(), stream, glossIter.value().id() );
    }

    QStringListIterator tagIter( mText->mDbAdapter->grammaticalTagsForAllomorph( allomorph.id() ) );
    while( tagIter.hasNext() )
    {
        stream->writeEmptyElement("http://www.adambaker.org/gloss.php", "grammatical-tag");
        stream->writeAttribute("http://www.adambaker.org/gloss.php", "name", tagIter.next() );
    }

    stream->writeEndElement(); // morph

    return true;
}

bool FlexTextWriter::serializeLanguages(QXmlStreamWriter *stream) const
{
    stream->writeStartElement("languages");
    QList<WritingSystem> ws = mText->mDbAdapter->writingSystems();
    for( int i=0; i<ws.count(); i++ )
    {
        stream->writeStartElement("language");
        stream->writeAttribute("lang", ws.at(i).flexString() );
        stream->writeAttribute("font", ws.at(i).fontFamily() );
        stream->writeAttribute("http://www.adambaker.org/gloss.php","font-size", QString("%1").arg(ws.at(i).fontSize()) );
        if( ws.at(i).layoutDirection() == Qt::RightToLeft )
            stream->writeAttribute("RightToLeft", "true" );
        if( mText->mBaselineWritingSystem.flexString() == ws.at(i).flexString() )
            stream->writeAttribute("http://www.adambaker.org/gloss.php","is-baseline", "true" );

        stream->writeEndElement(); // language
    }
    stream->writeEndElement(); // languages

    return true;
}

void FlexTextWriter::serializeItem(const QString & type, const WritingSystem & ws, const QString & text , QXmlStreamWriter *stream, qlonglong id) const
{
    stream->writeStartElement("item");
    if( id != -1 )
        stream->writeAttribute("http://www.adambaker.org/gloss.php", "id", QString("%1").arg(id) );
    stream->writeAttribute("type",type);
    stream->writeAttribute("lang",ws.flexString());
    stream->writeCharacters(text);
    stream->writeEndElement();
}

void FlexTextWriter::serializeItemNonVerbose(const QString & type, const WritingSystem & ws, QXmlStreamWriter *stream, qlonglong id ) const
{
    stream->writeStartElement("item");
    if( id != -1 )
        stream->writeAttribute("http://www.adambaker.org/gloss.php", "id", QString("%1").arg(id) );
    stream->writeAttribute("type",type);
    stream->writeAttribute("lang",ws.flexString());
    stream->writeEndElement();
}

