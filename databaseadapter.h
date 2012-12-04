/*!
  \class DatabaseAdapter
  \ingroup Data
  \brief A database adapter to provide an interface to the SQLite database.
*/

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

    //! \brief Returns a list of possible interpretations of the text form TextBit \a bit
    QList<qlonglong> candidateInterpretations(const TextBit & bit) const;

    //! \brief Returns a list of possible interpretations of the given text and gloss forms
    QList<qlonglong> candidateInterpretations(const QList<TextBit> & textForms , const QList<TextBit> & glossForms );

    //! \brief Creates a new interpretation of the baseline TextBit \a bit and returns the database index of the Interpreation
    qlonglong newInterpretation( const TextBit & bit );

    //! \brief Creates a new interpretation with the given text forms and glosses
    qlonglong newInterpretation(const QList<TextBit> & textForms , const QList<TextBit> & glossForms );

    QString getInterpretationGloss(qlonglong id, const WritingSystem & ws) const;
    QString getInterpretationTextForm(qlonglong id, const WritingSystem & ws) const;
    QString getInterpretationMorphologicalAnalysis(qlonglong id, const WritingSystem & ws) const;

    QList<TextBit> getInterpretationGlosses(qlonglong id) const;
    QList<TextBit> getInterpretationTextForms(qlonglong id) const;

    //! \brief Adds the writing system to the database
    void addWritingSystem(const QString & flexString, const QString & fontFamily, Qt::LayoutDirection layoutDirection);

    //! \brief Returns a pointer to the WritingSystem specified by \a flexString, or 0 if none exists.
    WritingSystem writingSystem(const QString &  flexString) const;

    QList<WritingSystem*> writingSystems() const;
    bool writingSystemExists(const QString & flexstring) const;
    QSqlDatabase* db();

    void close();

private:
    QSqlDatabase mDb;
    void createTables();

    QString mFilename;

signals:

public slots:
    void updateInterpretationGloss( const TextBit & bit );

    //! \brief Updates a text form in the database, where the id of the textform is the id field of \a bit
    void updateInterpretationTextForm( const TextBit & bit );

    void updateInterpretationMorphologicalAnalysis( const TextBit & bit , const QString & splitString );

};

#endif // DATABASEADAPTER_H
