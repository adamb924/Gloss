/*!
  \class Project
  \ingroup DataStructures
  \brief A data class holding data for a project.
*/

#ifndef PROJECT_H
#define PROJECT_H

#include <QtSql>
#include <QHash>
#include <QList>

#include "writingsystem.h"
#include "text.h"

class InterlinearItemType;
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

    DatabaseAdapter* dbAdapter();

    QStringList* textPaths();

    bool save();

    Text* newText(const QString & name, const WritingSystem & ws, const QString &content = QString() , bool openText = true );
    Text* textFromFlexText(const QString & filePath, const WritingSystem & ws);
    Text* textFromFlexText(const QString & filePath);

    //! \brief Returns a list of text names, sorted alphabetically.
    QStringList textNames() const;

    QDir getTempDir() const;

    void removeTempDirectory();

    QHash<QString,Text*>* texts();

    bool openText(const QString & name);

    QString filepathFromName(const QString & name) const;

public slots:

private:
    DatabaseAdapter *mDbAdapter;
    QString mDatabaseFilename;
    QString mDatabasePath;

    QString mProjectPath;

    void readTextPaths();
    bool maybeDelete(QDir tempDir);

    QString tempDirName() const;


    //! \brief Paths of all texts in the temp directory
    QStringList mTextPaths;

    //! \brief A hash containing all "opened" texts, keyed by name
    QHash<QString,Text*> mTexts;
};

#endif // PROJECT_H
