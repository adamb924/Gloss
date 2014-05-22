/*!
  \class AllomorphModel
  \ingroup GUIModel
  \brief This class is a model for displaying the allomorphs of a lexical entry. Currently it is only used by LexiconEdit.
*/

#ifndef ALLOMORPHMODEL_H
#define ALLOMORPHMODEL_H

#include <QSqlQueryModel>

class DatabaseAdapter;

class AllomorphModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    AllomorphModel(const DatabaseAdapter * dbAdapter, QObject *parent = 0);

signals:

public slots:
    void setLexicalEntry( qlonglong lexicalEntryId );

private:
    const DatabaseAdapter * mDbAdapter;
    qlonglong mLexicalEntryId;
};

#endif // ALLOMORPHMODEL_H
