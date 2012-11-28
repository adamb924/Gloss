#ifndef DATABASEADAPTER_H
#define DATABASEADAPTER_H

#include <QObject>
#include <QtSql>

class TextBit;
class WritingSystem;

class DatabaseAdapter : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseAdapter(const QString & filename, QObject *parent = 0);
    ~DatabaseAdapter();

    void initialize(QString filename);

    QList<qlonglong> candidateInterpretationsPhonetic(const QString & form) const;
    QList<qlonglong> candidateInterpretationsOrthographic(const QString & form) const;

    qlonglong newInterpretationFromOrthography( const TextBit & bit );
    qlonglong newInterpretationFromPhonetic( const TextBit & bit );

    QString getInterpretationGloss(qlonglong id, WritingSystem *ws) const;
    QString getInterpretationTranscription(qlonglong id, WritingSystem *ws) const;
    QString getInterpretationOrthography(qlonglong id, WritingSystem *ws) const;
    QString getInterpretationMorphologicalAnalysis(qlonglong id, WritingSystem *ws) const;

    void addWritingSystem(const QString & flexString, const QString & fontFamily, Qt::LayoutDirection layoutDirection);

    WritingSystem* writingSystem(QString code);
    QList<WritingSystem*> writingSystems() const;
    bool writingSystemExists(const QString & flexstring) const;
    QSqlDatabase* db();

    QList<qlonglong> getInterpretation();

private:
    QSqlDatabase mDb;
    void createTables();

signals:

public slots:
    void updateInterpretationGloss( const TextBit & bit );
    void updateInterpretationTranscription( const TextBit & bit );
    void updateInterpretationOrthography( const TextBit & bit );
    void updateInterpretationMorphologicalAnalysis( const TextBit & bit , const QString & splitString );

};

#endif // DATABASEADAPTER_H
