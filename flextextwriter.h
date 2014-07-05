/*!
  \class FlexTextWriter
  \ingroup IO
  \brief This object writes the data in a Text object to a .flextext file. Instantiated in Text.
*/

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
    FlexTextWriter(Text *text);

    bool writeFile( const QString & filename );

    void setVerboseOutput(bool value);
    void setIncludeGlossNamespace(bool value);

private:
    Text *mText;
    bool mVerboseOutput;
    bool mIncludeGlossNamespace;
    QXmlStreamWriter * stream;

    //! \brief Serialize the interlinear text (i.e., <interlinear-text> to the XML stream
    bool serializeInterlinearText() const;

    bool serializeGlossItem(GlossItem *glossItem) const;

    bool serializePunctuation(GlossItem *glossItem) const;

    //! \brief Serialize the interlinear text (i.e., <interlinear-text> to the XML stream)
    bool serializeMorphemes(GlossItem *glossItem) const;

    //! \brief Serialize the allomorph (i.e., <morph> in the XML stream)
    bool serializeAllomorph(const Allomorph & allomorph) const;

    //! \brief Serialize the allomorph (i.e., <morph> in the XML stream) without including all of the database information
    bool serializeAllomorphNonverbose(const Allomorph & allomorph) const;

    //! \brief Serialize the languages.
    bool serializeLanguages() const;

    //! \brief Write an &lt;item&gt; to the text stream, with specified attributes and text content
    void serializeItem(const QString & type, const WritingSystem & ws, const QString & text , qlonglong id = -1) const;

    //! \brief Write an &lt;item&gt; to the text stream, with specified attributes and text content
    void serializeItemNonVerbose(const QString & type, const WritingSystem & ws, qlonglong id = -1) const;

    //! \brief If the Gloss namespace is being included in the output, writes an attribute with name \a name and value \a value.
    void writeNamespaceAttribute(const QString & name, const QString & value) const;
};

#endif // FLEXTEXTWRITER_H
