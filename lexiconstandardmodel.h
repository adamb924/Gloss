#ifndef LEXICONSTANDARDMODEL_H
#define LEXICONSTANDARDMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QSqlQuery>

#include "writingsystem.h"

class DatabaseAdapter;

class LexiconStandardModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    LexiconStandardModel(const DatabaseAdapter * dbAdapter, QObject *parent = 0);

    enum Type { Gloss, CitationForm, Other };
    enum Data { LexicalEntryId = Qt::UserRole, TextFormGlossId = Qt::UserRole + 1 };

    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

signals:

public slots:

private:
    void refreshQuery();
    QString buildQueryString() const;
    QString buildCountQueryString() const;
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    inline QString glossTable(int number) const { return QString("Gloss%1").arg(number); }
    inline QString citationFormTable(int number) const { return QString("Citation%1").arg(number); }

    const DatabaseAdapter * mDbAdapter;

    void typeFromColumn( const int col, Type & type , int & index );

    int columnForCitationForm( const int index ) const;
    int columnForGloss( const int index ) const;

    QList<WritingSystem> mGlossFields;
    QList<WritingSystem> mCitationFormFields;
    QSqlQuery mQuery;
    qlonglong mRowCount;
    QString mQueryString;
};

#endif // LEXICONSTANDARDMODEL_H
