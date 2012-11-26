#ifndef PROJECT_H
#define PROJECT_H

#include <QtSql>
#include <QHash>
#include <QList>

#include "writingsystem.h"

class GlossLine;
class TextBit;
class DatabaseAdapter;

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

    bool readFromFile(QString filename);

    QList<GlossLine> glossLines();

    DatabaseAdapter* dbAdapter();

public slots:

private:
    DatabaseAdapter *mDbAdapter;
    QStringList mTextPaths;
    QString mDatabaseFilename;

    QString mProjectPath;
    QString mTempPath;

    void readTextPaths();
    QDir getTempDir();
};

#endif // PROJECT_H
