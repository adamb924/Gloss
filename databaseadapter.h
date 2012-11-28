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

    //! \brief Initializes the database at the path \a filename
    void initialize(QString filename);

    //! \brief Returns a list of possible interpretations of the baseline TextBit \a bit
    QList<qlonglong> candidateInterpretations(const TextBit & bit) const;

    //! \brief Creates a new interpretation of the baseline TextBit \a bit and returns the database index of the Interpreation
    qlonglong newInterpretation( const TextBit & bit );

    QString getInterpretationGloss(qlonglong id, WritingSystem *ws) const;
    QString getInterpretationTextForm(qlonglong id, WritingSystem *ws) const;
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

    //! \brief Updates a text form in the database, where the id of the textform is the id field of \a bit
    void updateInterpretationTextForm( const TextBit & bit );

    void updateInterpretationMorphologicalAnalysis( const TextBit & bit , const QString & splitString );

};

#endif // DATABASEADAPTER_H
