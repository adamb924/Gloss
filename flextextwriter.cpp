#include "flextextwriter.h"

#include "text.h"
#include "phrase.h"
#include "glossitem.h"
#include "allomorph.h"
#include "databaseadapter.h"
#include "project.h"
#include "syntacticanalysis.h"
#include "syntacticanalysiselement.h"
#include "annotation.h"

#include <QFile>
#include <QXmlStreamWriter>
#include <QProgressDialog>
#include <QtDebug>

FlexTextWriter::FlexTextWriter(Text *text) :
    mText(text),
    mVerboseOutput(false),
    mIncludeGlossNamespace(true),
    mStream(0)
{
}

bool FlexTextWriter::writeFile( const QString & filename )
{
    QFile outFile(filename);
    if( !outFile.open(QFile::WriteOnly | QFile::Text) )
        return false;

    mStream = new QXmlStreamWriter(&outFile);
    mStream->setCodec("UTF-8");
    mStream->setAutoFormatting(true);
    if( mIncludeGlossNamespace )
    {
        mStream->writeNamespace("http://www.adambaker.org/gloss.php", "abg" );
    }

    mStream->writeStartDocument();
    mStream->writeStartElement("document");
    mStream->writeAttribute("version", "2");

    if( mIncludeGlossNamespace )
    {
        writeNamespaceAttribute("export-filename", mText->exportFilename() );
    }

    serializeInterlinearText();

    mStream->writeEndElement(); // document
    mStream->writeEndDocument();

    delete mStream;

    return true;
}

bool FlexTextWriter::serializeInterlinearText() const
{
    mStream->writeStartElement("interlinear-text");

    writeNamespaceAttribute( "baseline-writing-system", mText->project()->metaLanguage().flexString() );

    if( !mText->mAudioFileURL.isEmpty() )
        writeNamespaceAttribute( "audio-file", mText->mAudioFileURL.toString() );

    if( !mText->mName.isEmpty() )
        serializeItem( "title" , mText->project()->metaLanguage() , mText->mName  );
    if( !mText->mComment.isEmpty() )
        serializeItem( "comment" , mText->project()->metaLanguage() , mText->mComment  );

    mStream->writeStartElement("paragraphs");

    QProgressDialog progress( QObject::tr("Saving text %1...").arg(mText->mName), QString(), 0, mText->mPhrases.count(), 0);
    progress.setWindowModality(Qt::WindowModal);

    int count = 0;
    foreach( Phrase* phrase, mText->mPhrases )
    {
        count++;

        progress.setValue(count);

        // this rather profligate nesting seems to be a feature of flextext files
        mStream->writeStartElement("paragraph");
        mStream->writeStartElement("phrases");
        mStream->writeStartElement("phrase");

        if( !phrase->interval()->isNull() )
        {
            writeNamespaceAttribute( "annotation-start", QString("%1").arg(phrase->interval()->start()) );
            writeNamespaceAttribute( "annotation-end", QString("%1").arg(phrase->interval()->end()) );
        }

        serializeItem("segnum", mText->project()->metaLanguage(), QString("%1").arg(count)  );

        mStream->writeStartElement("words");
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

        mStream->writeEndElement(); // words

        // phrase-level glosses
        TextBitHashIterator iter = phrase->glosses();
        while (iter.hasNext())
        {
            iter.next();
            serializeItem("gls",iter.key(),iter.value().text());
        }

        mStream->writeEndElement(); // phrase
        mStream->writeEndElement(); // phrases
        mStream->writeEndElement(); // paragraph
    }

    progress.setValue(mText->mPhrases.count());

    mStream->writeEndElement(); // paragraphs

    if( mIncludeGlossNamespace )
    {
        QStringList keys = mText->syntacticAnalyses()->keys();
        for(int i=0; i<keys.count(); i++)
        {
            serializeGrammaticalAnalysis( mText->syntacticAnalyses()->value(keys.at(i)) );
        }
    }

    serializeLanguages();

    mStream->writeEndElement(); // interlinear-text

    return true;
}

bool FlexTextWriter::serializeGlossItem(GlossItem *glossItem) const
{
    mStream->writeStartElement("word");

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

    serializeMorphemes( glossItem );

    // custom annotations
    if( mIncludeGlossNamespace )
    {
        QHashIterator<QString,Annotation> annIter = glossItem->annotations();
        while ( annIter.hasNext() )
        {
            annIter.next();
            serializeAnnotation( annIter.key(), annIter.value() );
        }
    }
    mStream->writeEndElement(); // word

    return true;
}

bool FlexTextWriter::serializePunctuation(GlossItem *glossItem) const
{
    mStream->writeStartElement("word");
    writeNamespaceAttribute( "id", QString("%1").arg(glossItem->id()) );
    serializeItem("punct", glossItem->baselineWritingSystem(), glossItem->baselineText().text(), glossItem->baselineText().id() );
    mStream->writeEndElement(); // word
    return true;
}

bool FlexTextWriter::serializeGrammaticalAnalysis(const SyntacticAnalysis * analysis) const
{
    mStream->writeStartElement("http://www.adambaker.org/gloss.php", "syntactic-analysis");
    writeNamespaceAttribute("name", analysis->name());
    writeNamespaceAttribute("lang", analysis->writingSystem().flexString() );
    writeNamespaceAttribute("closed-vocabulary", analysis->closedVocabulary() ? "yes" : "no" );

    for(int i=0; i<analysis->elements()->count(); i++)
    {
        serializeGrammaticalElement( analysis->elements()->at(i) );
    }

    mStream->writeEndElement(); // syntactic-analysis
    return true;
}

bool FlexTextWriter::serializeGrammaticalElement(const SyntacticAnalysisElement * element) const
{
    if( element->isTerminal() )
    {
        mStream->writeEmptyElement("http://www.adambaker.org/gloss.php","terminal");
        writeNamespaceAttribute("guid", element->allomorph()->guid() );
    }
    else /// constituent
    {
        mStream->writeStartElement("http://www.adambaker.org/gloss.php","constituent");
        writeNamespaceAttribute("label", element->label() );
        for(int i=0; i<element->children()->count(); i++)
        {
            serializeGrammaticalElement( element->children()->at(i) );
        }
        mStream->writeEndElement(); // constituent
    }
    return true;
}

bool FlexTextWriter::serializeMorphemes(GlossItem *glossItem) const
{
    QList<WritingSystem> analysisLanguages = glossItem->nonEmptyMorphologicalAnalysisLanguages();
    foreach( WritingSystem ws, analysisLanguages )
    {
        const MorphologicalAnalysis * analysis = glossItem->morphologicalAnalysis( ws );
        if( ! analysis->isEmpty() )
        {
            mStream->writeStartElement("morphemes");
            writeNamespaceAttribute( "lang", ws.flexString() );

            AllomorphPointerIterator iter = analysis->allomorphIterator();
            while(iter.hasNext())
            {
                mVerboseOutput ? serializeAllomorph( *iter.next() ) : serializeAllomorphNonverbose( *iter.next() );
            }
            mStream->writeEndElement(); // morphemes
        }
    }
    return true;
}

bool FlexTextWriter::serializeAllomorph(const Allomorph & allomorph) const
{
    mStream->writeStartElement("morph");

    mStream->writeAttribute("type", allomorph.typeString());
    writeNamespaceAttribute( "id", QString("%1").arg(allomorph.id()) );
    writeNamespaceAttribute( "guid" , allomorph.guid() );

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
            mStream->writeEmptyElement("http://www.adambaker.org/gloss.php", "grammatical-tag");
            mStream->writeAttribute("http://www.adambaker.org/gloss.php", "name", tagIter.next() );
        }
    }

    mStream->writeEndElement(); // morph

    return true;
}

bool FlexTextWriter::serializeAllomorphNonverbose(const Allomorph &allomorph) const
{
    mStream->writeEmptyElement("morph");
    writeNamespaceAttribute( "id", QString("%1").arg(allomorph.id()) );
    writeNamespaceAttribute( "guid" , allomorph.guid() );

    return true;
}

bool FlexTextWriter::serializeAnnotation(const QString &key, const Annotation &annotation) const
{
    if( !annotation.text().text().isEmpty() )
    {
        mStream->writeStartElement("http://www.adambaker.org/gloss.php", "annotation");
        mStream->writeAttribute("key", key );

        mStream->writeStartElement("http://www.adambaker.org/gloss.php", "annotation-text");
        mStream->writeAttribute("lang", annotation.text().writingSystem().flexString() );
        mStream->writeCharacters(annotation.text().text());
        mStream->writeEndElement();

        mStream->writeStartElement("http://www.adambaker.org/gloss.php", "annotation-header");
        mStream->writeAttribute("lang", annotation.header().writingSystem().flexString() );
        mStream->writeCharacters(annotation.header().text());
        mStream->writeEndElement();

        mStream->writeEndElement(); // annotation
    }

    return true;
}

bool FlexTextWriter::serializeLanguages() const
{
    mStream->writeStartElement("languages");
    QList<WritingSystem> ws = mText->mDbAdapter->writingSystems();
    for( int i=0; i<ws.count(); i++ )
    {
        mStream->writeStartElement("language");
        mStream->writeAttribute("lang", ws.at(i).flexString() );
        mStream->writeAttribute("font", ws.at(i).fontFamily() );
        writeNamespaceAttribute("font-size", QString("%1").arg(ws.at(i).fontSize()) );
        if( ws.at(i).layoutDirection() == Qt::RightToLeft )
            mStream->writeAttribute("RightToLeft", "true" );
        if( mText->mBaselineWritingSystem.flexString() == ws.at(i).flexString() )
            writeNamespaceAttribute("is-baseline", "true" );

        mStream->writeEndElement(); // language
    }
    mStream->writeEndElement(); // languages

    return true;
}

void FlexTextWriter::serializeItem(const QString & type, const WritingSystem & ws, const QString & text , qlonglong id) const
{
    mStream->writeStartElement("item");
    if( id != -1 )
        writeNamespaceAttribute( "id", QString("%1").arg(id) );
    mStream->writeAttribute("type",type);
    mStream->writeAttribute("lang",ws.flexString());
    mStream->writeCharacters(text);
    mStream->writeEndElement();
}

void FlexTextWriter::serializeItemNonVerbose(const QString & type, const WritingSystem & ws, qlonglong id ) const
{
    mStream->writeStartElement("item");
    if( id != -1 )
        writeNamespaceAttribute( "id", QString("%1").arg(id) );
    mStream->writeAttribute("type",type);
    mStream->writeAttribute("lang",ws.flexString());
    mStream->writeEndElement();
}

void FlexTextWriter::setVerboseOutput(bool value)
{
    mVerboseOutput = value;
}

void FlexTextWriter::setIncludeGlossNamespace(bool value)
{
    mIncludeGlossNamespace = value;
}

void FlexTextWriter::writeNamespaceAttribute(const QString &name, const QString &value) const
{
    if( mIncludeGlossNamespace )
    {
        mStream->writeAttribute("http://www.adambaker.org/gloss.php", name, value );
    }
}

