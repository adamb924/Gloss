/*!
  \class Text
  \ingroup Data
  \brief A data class holding data for a text.
*/

#ifndef TEXT_H
#define TEXT_H

#include <QObject>
#include <QString>
#include <QList>
#include <QUrl>
#include <QFileInfo>

#include "textbit.h"
#include "glossitem.h"
#include "phrase.h"
#include "flextextreader.h"

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

    friend class FlexTextReader;
    friend class FlexTextWriter;
    friend class FlexTextImporter;

public:
    enum MergeTranslationResult { Success, MergeStuckOldFileDeleted, MergeStuckOldFileStillThere, XslTranslationError };
    enum MergeEafResult { MergeEafSuccess, MergeEafFailure, MergeEafWrongNumberOfAnnotations };
    enum FlexTextReadResult { FlexTextReadSuccess, FlexTextReadBaselineNotFound, FlexTextReadXmlReadError, FlexTextReadNoAttempt };

    Text();
    Text(const WritingSystem & ws, const QString & name, Project *project);
    Text(const QString & filePath, Project *project);
    Text(const QString & filePath, const WritingSystem &, Project *project);
    ~Text();

    QString name() const;
    void setName(const QString & name);

    QString comment() const;
    void setComment(const QString & comment);

    WritingSystem baselineWritingSystem() const;
    void setBaselineWritingSystem(const WritingSystem & ws);

    QString baselineText() const;
    void setBaselineText(const QString & text);

    const Project * project() const;

    QList<Phrase*>* phrases();

    void saveText(bool verboseOutput, bool saveAnyway = false);

    Text::MergeTranslationResult mergeTranslation(const QString & filename, const WritingSystem & ws );

    Text::MergeEafResult mergeEaf(const QString & filename );

    static QString textNameFromPath(const QString & path)
    {
        QFileInfo info(path);
        return info.baseName();
    }

    //! \brief Returns true if the Text is valid, otherwise false. A Text can be invalid if there has been some error, for instance.
    bool isValid() const;

    QString audioFilePath() const;
    void setAudioFilePath(const QUrl & path);

    void setBaselineTextForLine( int i, const QString & text );
    QString baselineTextForLine( int i );

    void setSound(const QUrl & filename);

    //! \brief Plays the sound for the given 0-indexed line number, or returns an error.
    bool playSoundForLine( int lineNumber );

    FlexTextReader::Result readResult() const;

    void findGlossItemLocation(GlossItem *glossItem, int & line, int & position) const;

public slots:
    void setBaselineFromGlossItems();
    void markAsChanged();
    void removeLine( int lineNumber );
    void setFollowingInterpretations( GlossItem *glossItem );
    void replaceFollowing(GlossItem *glossItem, const QString & searchFor );
    void baselineSearchReplace( const TextBit & search , const TextBit & replace );
    void matchFollowingTextForms(GlossItem *glossItem, const WritingSystem & ws );
    void matchFollowingGlosses(GlossItem *glossItem, const WritingSystem & ws );

private:
    FlexTextReader::Result mReadResult;
    bool mChanged;

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

    void setLineOfGlossItems(Phrase *phrase , const QString & line );

private slots:
    void requestGuiRefresh( Phrase * phrase );

signals:
    void baselineTextChanged(const QString & baselineText);
    void glossItemsChanged();
    void idChanged(TextBit *b, qlonglong oldId);
    void phraseRefreshNeeded( int lineNumber );
};

#endif // TEXT_H
