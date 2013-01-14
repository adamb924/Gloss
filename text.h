/*!
  \class Text
  \ingroup DataStructures
  \brief A data class holding data for a text.
*/

#ifndef TEXT_H
#define TEXT_H

#include <QObject>
#include <QString>
#include <QList>
#include <QUrl>

#include "textbit.h"
#include "glossitem.h"
#include "phrase.h"

class WritingSystem;
class Project;
class QFile;
class QXmlStreamWriter;
class QDir;
class DatabaseAdapter;
class QUrl;
class Sound;

class Text : public QObject
{
    Q_OBJECT
public:
    enum MergeTranslationResult { Success, MergeStuckOldFileDeleted, MergeStuckOldFileStillThere, XslTranslationError };
    enum MergeEafResult { MergeEafSuccess, MergeEafFailure, MergeEafWrongNumberOfAnnotations };

    Text();
    ~Text();
    Text(const WritingSystem & ws, const QString & name, Project *project);
    Text(const QString & filePath, Project *project);
    Text(const QString & filePath, const WritingSystem &, Project *project);

    QString name() const;
    void setName(const QString & name);

    QString comment() const;
    void setComment(const QString & comment);

    WritingSystem baselineWritingSystem() const;
    void setBaselineWritingSystem(const WritingSystem & ws);

    QString baselineText() const;
    void setBaselineText(const QString & text);

    QList<Phrase*>* phrases();

    void saveText() const;

    Text::MergeTranslationResult mergeTranslation(const QString & filename, const WritingSystem & ws );

    Text::MergeEafResult mergeEaf(const QString & filename );

    QString textNameFromPath(const QString & path) const;

    //! \brief Serialize the text to an XML file
    bool serialize(const QString & filename) const;

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

    //! \brief Reads the given flextext file to set the baseline writing system for the text
    bool setBaselineWritingSystemFromFile(const QString & filePath );

    //! \brief Returns true if the Text is valid, otherwise false. A Text can be invalid if there has been some error, for instance.
    bool isValid() const;

    QString audioFilePath() const;
    void setAudioFilePath(const QUrl & path);

    void setBaselineTextForLine( int i, const QString & text );
    QString baselineTextForLine( int i );


    Sound* sound();
    void setSound(const QUrl & filename);

    //! \brief Plays the sound for the given 0-indexed line number, or returns an error.
    bool playSoundForLine( int lineNumber );


private:
    Sound *mSound;
    QString mName, mComment;
    QString mBaselineText;
    bool mValid;
    QUrl mAudioFileURL;

    Project *mProject;
    DatabaseAdapter *mDbAdapter;

    WritingSystem mBaselineWritingSystem;

    QList<Phrase*> mPhrases;

    void clearGlossItems();

    void setGlossItemsFromBaseline();
    void setBaselineFromGlossItems();

    void setLineOfGlossItems(Phrase *phrase , const QString & line );

    //! \brief Sets the text from the given file. Returns false if this fails.
    bool readTextFromFlexText(QFile *file, bool baselineInfoFromFile = false);

signals:
    void baselineTextChanged(const QString & baseline);
    void idChanged(TextBit *b, qlonglong oldId);
};

#endif // TEXT_H
