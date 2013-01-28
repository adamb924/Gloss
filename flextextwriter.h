#ifndef FLEXTEXTWRITER_H
#define FLEXTEXTWRITER_H

#include <QtGlobal>

class Text;
class QString;
class QXmlStreamWriter;
class Allomorph;
class GlossItem;
class WritingSystem;

class FlexTextWriter
{
public:
    FlexTextWriter(Text *text, bool verboseOutput);

    bool writeFile( const QString & filename );

private:
    Text *mText;
    bool mVerboseOutput;

    //! \brief Serialize the interlinear text (i.e., <interlinear-text> to the XML stream
    bool serializeInterlinearText(QXmlStreamWriter *stream) const;

    bool serializeGlossItem(GlossItem *glossItem, QXmlStreamWriter *stream) const;

    //! \brief Serialize the interlinear text (i.e., <interlinear-text> to the XML stream
    bool serializeMorphemes(GlossItem *glossItem, QXmlStreamWriter *stream) const;

    //! \brief Serialize the interlinear text (i.e., <interlinear-text> to the XML stream
    bool serializeAllomorph(const Allomorph & allomorph, QXmlStreamWriter *stream) const;

    //! \brief Serialize the languages.
    bool serializeLanguages(QXmlStreamWriter *stream) const;

    //! \brief Write an <item> to the text stream, with specified attributes and text content
    void serializeItem(const QString & type, const WritingSystem & ws, const QString & text , QXmlStreamWriter *stream, qlonglong id = -1) const;

};

#endif // FLEXTEXTWRITER_H
