/*!
  \class SearchQueryModel
  \ingroup GUIModel
  \ingroup Search
  \brief A model class for performing XQuery searches. This is used in MainWindow in conjunction with SearchQueryView.
*/

#ifndef SEARCHQUERYMODEL_H
#define SEARCHQUERYMODEL_H

#include <QStandardItemModel>
#include "focus.h"

class QXmlResultItems;

class SearchQueryModel : public QStandardItemModel
{
public:
    SearchQueryModel( const QString & queryString, const QSet<QString>* textPaths, QObject * parent, const QList<Focus> & focus = QList<Focus>() );

private:
    QString mQuery;
    QList<Focus> mFocus;

    bool query( QStandardItem *parentItem, const QString & filename );
};

#endif // SEARCHQUERYMODEL_H
