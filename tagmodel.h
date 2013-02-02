#ifndef TAGMODEL_H
#define TAGMODEL_H

#include <QStringListModel>

class DatabaseAdapter;

class TagModel : public QStringListModel
{
    Q_OBJECT
public:
    TagModel(const DatabaseAdapter * dbAdapter, QObject *parent = 0);

signals:

public slots:
    void setLexicalEntry( qlonglong lexicalEntryId = -1 );

private:
    const DatabaseAdapter * mDbAdapter;
    qlonglong mLexicalEntryId;
};

#endif // TAGMODEL_H
