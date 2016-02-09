/*!
  \class IndexSearchModel
  \ingroup GUIModel
  \brief This is a model containing the results of an index-based search. It is instantiated in MainWindow when the user does a search from the search dock (SearchForm). This model is created and displayed in a SearchQueryView.
*/

#ifndef INDEXSEARCHMODEL_H
#define INDEXSEARCHMODEL_H

#include <QStandardItemModel>
#include <QSqlQuery>
#include <QList>

class Focus;

class IndexSearchModel : public QStandardItemModel
{
public:
    IndexSearchModel( QSqlQuery query , const QList<Focus> & focus = QList<Focus>() );

    QString resultSummary() const;

private:
    QSqlQuery mQuery;
};

#endif // INDEXSEARCHMODEL_H
