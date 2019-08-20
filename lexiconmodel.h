/*!
  \class LexiconModel
  \ingroup GUIModel
  \ingroup Lexicon
  \brief This is a QSqlQueryModel subclass for displaying the lexical entries in a project. It is subclassed in order to make the data editable. It is used in LexiconEdit.
*/

#ifndef LEXICONMODEL_H
#define LEXICONMODEL_H

#include <QSqlQueryModel>

class DatabaseAdapter;
class AllTagsModel;
class Project;

#include "writingsystem.h"

class LexiconModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    enum Type { Gloss, CitationForm, MorphologicalType, Other };

    LexiconModel(const AllTagsModel * allTags, const Project *project, QObject *parent = nullptr);

    Type typeFromColumn(int col);
    WritingSystem writingSystemFromColumn(int col);

    bool setData(const QModelIndex &index, const QVariant &value, int role);

signals:

public slots:
    void refreshQuery();

private:
    QString buildQueryString();

    qlonglong lexicalEntryId( const QModelIndex & modelIndex ) const;

    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    inline QString glossTable(qlonglong number) const { return QString("Gloss%1").arg(number); }
    inline QString citationFormTable(qlonglong number) const { return QString("Citation%1").arg(number); }

    QString mQueryString;
    const DatabaseAdapter * mDbAdapter;
    const Project * mProject;
    const AllTagsModel * mAllTags;
    QList<WritingSystem> mGlossFields;
    QList<WritingSystem> mCitationFormFields;
    QList<bool> mEditable;
};

#endif // LEXICONMODEL_H
