#ifndef LEXICONMODEL_H
#define LEXICONMODEL_H

#include <QSqlQueryModel>

class DatabaseAdapter;

#include "writingsystem.h"

class LexiconModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    enum Type { Gloss, CitationForm, MorphologicalType, Other };

    LexiconModel(const DatabaseAdapter * dbAdapter, QObject *parent = 0);

signals:

public slots:

private:
    QString buildQueryString();
    void refreshQuery();
    Type typeFromColumn(int col);

    qlonglong lexicalEntryId( const QModelIndex & modelIndex ) const;

    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    inline QString glossTable(int number) const { return QString("Gloss%1").arg(number); }
    inline QString citationFormTable(int number) const { return QString("Citation%1").arg(number); }

    QString mQueryString;
    const DatabaseAdapter * mDbAdapter;
    QList<WritingSystem> mGlossFields;
    QList<WritingSystem> mCitationFormFields;
    QList<bool> mEditable;
};

#endif // LEXICONMODEL_H
