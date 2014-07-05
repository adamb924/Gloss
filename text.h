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
class SyntacticAnalysis;

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

    const Project * project() const;

    QList<Phrase*>* phrases();

    void writeTextTo(const QString & path, bool verboseOutput, bool morphologicalAnalysis, bool glossNamespace);

    //! @todo There is probably a better way to write the interface for this method.
    void saveText(bool verboseOutput, bool morphologicalAnalysis, bool glossNamespace, bool saveAnyway);

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

    void setGlossItemsFromBaseline(const QString &content);

    QHash<QString, SyntacticAnalysis*>* syntacticAnalyses();

public slots:
    void markAsChanged();
    void removeLine( int lineNumber );
    void setFollowingInterpretations( GlossItem *glossItem );
    void replaceFollowing(GlossItem *glossItem, const QString & searchFor );
    void baselineSearchReplace( const TextBit & search , const TextBit & replace );
    void matchFollowingTextForms(GlossItem *glossItem, const WritingSystem & ws );
    void matchFollowingGlosses(GlossItem *glossItem, const WritingSystem & ws );

private:
    Sound *mSound;
    FlexTextReader::Result mReadResult;
    bool mValid;
    bool mChanged;

    QString mName;
    WritingSystem mBaselineWritingSystem;
    Project *mProject;
    DatabaseAdapter *mDbAdapter;

    QHash<QString, SyntacticAnalysis*> mSyntacticAnalyses;

    QString mComment;
    QUrl mAudioFileURL;

    QList<Phrase*> mPhrases;


    void clearGlossItems();


    void setLineOfGlossItems(Phrase *phrase , const QString & line );

private slots:
    void requestGuiRefresh( Phrase * phrase );
    void registerPhrasalGlossChange(Phrase *thisPhrase, const TextBit & bit);
    void removePhrase( Phrase * phrase );

signals:
    void glossItemsChanged();
    void idChanged(TextBit *b, qlonglong oldId);
    void phraseRefreshNeeded( int lineNumber );
    void guiRefreshRequest();
    void phrasalGlossChanged( int lineNumber, const TextBit & bit);
};

#endif // TEXT_H
