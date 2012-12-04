/*!
  \class Project
  \ingroup Data
  \brief A data class holding data for a project.
*/

#ifndef PROJECT_H
#define PROJECT_H

#include <QtSql>
#include <QHash>
#include <QList>

#include "writingsystem.h"
#include "text.h"

class GlossLine;
class TextBit;
class DatabaseAdapter;

class Project : public QObject
{
    Q_OBJECT
public:
    Project();
    ~Project();

    bool create(QString filename);
    bool readFromFile(QString filename);

    QList<GlossLine> glossLines() const;

    DatabaseAdapter* dbAdapter();

    QStringList* textPaths();

    bool save();

    Text* newBlankText(const QString & name, const WritingSystem & ws);
    Text* textFromFlexText(const QString & filePath, const WritingSystem & ws);
    Text* textFromFlexText(const QString & filePath);

    QDir getTempDir();

    void removeTempDirectory();

    QHash<QString,Text*>* texts();

    bool openText(const QString & name);

public slots:

private:
    DatabaseAdapter *mDbAdapter;
    QString mDatabaseFilename;
    QString mDatabasePath;

    QString mProjectPath;
    QString mTempPath;

    void readTextPaths();


    bool maybeDelete(QDir tempDir);

    QString tempDirName() const;

    //! \brief Paths of all texts in the temp directory
    QStringList mTextPaths;

    //! \brief A hash containing all "opened" texts, keyed by name
    QHash<QString,Text*> mTexts;
};

#endif // PROJECT_H
