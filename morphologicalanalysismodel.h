/*!
  \class MorphologicalAnalysisModel
  \ingroup GUIModel
  \ingroup LexiconEdit
  \brief This class provides a model for displaying the morphological analyses associated with a particular lexical item. It is instantiated in LexiconEdit.
*/

#ifndef MORPHOLOGICALANALYSISMODEL_H
#define MORPHOLOGICALANALYSISMODEL_H

#include <QSqlQueryModel>

#include "writingsystem.h"

class DatabaseAdapter;

class MorphologicalAnalysisModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    MorphologicalAnalysisModel(const DatabaseAdapter * dbAdapter, QObject *parent = 0);

public slots:
    void setLexicalEntry( qlonglong lexicalEntryId );
    void setTextFormWritingSystem( const WritingSystem & ws );
    void setGlossWritingSystem( const WritingSystem & ws );

signals:

public slots:

private:
    const DatabaseAdapter * mDbAdapter;
    WritingSystem mTextFormWs;
    WritingSystem mGlossWs;
    qlonglong mLexicalEntryId;
};

#endif // MORPHOLOGICALANALYSISMODEL_H
