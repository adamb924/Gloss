/*!
  \class Allomorph
  \ingroup Data
  \brief This class represents an allomorph. Typically Allomorph objects would be contained within a MorphologicalAnalysis object.

  An Allomorph object corresponds to one row of the Allomorph SQL table.
*/

#ifndef ALLOMORPH_H
#define ALLOMORPH_H

#include "textbit.h"

#include <QRegExp>
#include <QUuid>

class Concordance;

class Allomorph : public QObject
{
    Q_OBJECT

public:
    enum Type { Stem,Prefix,Suffix,Infix,BoundStem,Proclitic,Enclitic,Simulfix,Suprafix,Null };

    Allomorph(QUuid guid = QUuid());
    Allomorph(qlonglong id, qlonglong lexicalEntryId, const TextBit & bit, Type type, QUuid guid = QUuid() );
    Allomorph(qlonglong id, qlonglong lexicalEntryId, const TextBit & bit, const TextBitHash & glosses , Type type , QUuid guid = QUuid());
    Allomorph(const Allomorph & other);
    Allomorph& operator=(const Allomorph & other);
    ~Allomorph();

    void connectToConcordance(Concordance * concordance);

    //! \brief Returns true if all members of \a other are identical (including the GUID)
    bool operator==(const Allomorph & other) const;

    //! \brief Returns true if all members of \a other are identical (excluding the GUID)
    bool equalExceptGuid(const Allomorph & other) const;

    bool equalExceptGuid(const Allomorph *other) const;

    //! \brief Returns the type of the allomorph as Allomorph::Type
    Type type() const;

    //! \brief Returns the type of the allomorph as a string
    QString typeString() const;

    //! \brief Returns the text of the allomorph (without punctuation indicators)
    QString text() const;

    //! \brief Returns the text of the allomorph, punctuation indicators indicating what kind of allomorph it is
    QString typeFormattedString() const;

    //! \brief Returns the string formatted with a - or a =, according to whether it is a proclitic, enclitic, prefix, suffix
    TextBit textBitForConcatenation() const;

    //! \brief Returns the text of the allomorph as a TextBit (including punctuation indicators)
    TextBit textBit() const;

    //! \brief Returns the WritingSystem of the allomorph
    WritingSystem writingSystem() const;

    //! \brief Returns true if the allomorph is a stem, otherwise false
    bool isStem() const;

    //! \brief Returns true if the allomorph is a clitic, otherwise false
    bool isClitic() const;

    //! \brief Return the gloss of the allomorph in WritingSystem \a ws
    TextBit gloss(const WritingSystem & ws) const;

    //! \brief Set all glosses of the allomorph to \a glosses. All existing glosses are lost
    void setGlosses(const TextBitHash & glosses);

    //! \brief Returns the a list of WritingSystem objects for which the allomorph has glosses
    QList<WritingSystem> glossLanguages() const;

    //! \brief Returns the glosses of the allomorph
    TextBitHash glosses() const;

    //! \brief Returns the id of the allomorph
    qlonglong id() const;

    qlonglong lexicalEntryId() const;

    //! \brief Set the id of the allomorph to \a id
    void setId(qlonglong id);

    //! \brief Set the type of the allomorph to \a t
    void setType( Type t );

    //! \brief Return the GUID in string format
    QString guid() const;

    //! \brief Set the GUID to \a guidString (where \a guidString is the string format of the QUuid)
    void setGuid( const QString & guidString );

    //! \brief Set the GUID to \a guidString (where \a guidString is the string format of the QUuid)
    void setGuid( const QUuid & guid );

    //! \brief Returns the type string for \a t
    static QString getTypeString(Type t);

    //! \brief Returns the Type, given its string represenation \a t
    static Type getType(QString t);

    //! \brief Returns a string representation of \a text, as if it were an allomorph of type \a t
    static QString getTypeFormatTextString( const QString & text, Type t );

    //! \brief Returns the Type of an allomorph, given the string representation \a string
    static Type typeFromFormattedString(const QString & string);

    //! \brief Returns \a string with punctuation stripped out
    static QString stripPunctuation( const QString & string );

signals:
    void allomorphDestroyed(Allomorph * allomorph);
    void glossesChanged(Allomorph * allomorph);

private:
    Type mType;
    TextBit mTextBit;
    qlonglong mId;
    qlonglong mLexicalEntryId;
    TextBitHash mGlosses;
    QUuid mGuid;
};

QDebug operator<<(QDebug dbg, const Allomorph &key);

#endif // ALLOMORPH_H
