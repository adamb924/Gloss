#ifndef PROJECT_H
#define PROJECT_H

#include <QtSql>
#include <QHash>
#include <QList>

#include "writingsystem.h"

class GlossLine;
class TextBit;

class Project : public QObject
{
    Q_OBJECT
public:
    Project();
    ~Project();

    enum GlossLineType {
        Orthography,
        Transcription,
        Gloss
    };
    enum BaselineMode { Orthographic, Phonetic };

    void initialize(QString filename);
    void readFromFile(QString filename);

    QHash<QString, WritingSystem*>* writingSystems();

    WritingSystem* writingSystem(QString code);

    QList<GlossLine> glossLines();

    QList<qlonglong> candidateInterpretationsPhonetic(const QString & form) const;
    QList<qlonglong> candidateInterpretationsOrthographic(const QString & form) const;

    qlonglong newInterpretationFromOrthography( const TextBit & bit );
    qlonglong newInterpretationFromPhonetic( const TextBit & bit );

    QString getInterpretationGloss(qlonglong id, WritingSystem *ws) const;
    QString getInterpretationTranscription(qlonglong id, WritingSystem *ws) const;
    QString getInterpretationOrthography(qlonglong id, WritingSystem *ws) const;
    QString getInterpretationMorphologicalAnalysis(qlonglong id, WritingSystem *ws) const;

public slots:
    void updateInterpretationGloss( const TextBit & bit );
    void updateInterpretationTranscription( const TextBit & bit );
    void updateInterpretationOrthography( const TextBit & bit );
    void updateInterpretationMorphologicalAnalysis( const TextBit & bit , const QString & splitString );

private:
    QSqlDatabase mDb;

    void createTables();


    QHash<QString, WritingSystem*> mWritingSystems;
};

#endif // PROJECT_H
