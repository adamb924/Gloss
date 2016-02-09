/*!
  \class XQueryModel
  \ingroup GUIModel
  \ingroup Search
  \brief A model class for performing XQuery searches. This is used in MainWindow in conjunction with SearchQueryView.
*/

#ifndef XQUERYMODEL_H
#define XQUERYMODEL_H

#include <QStandardItemModel>
#include "focus.h"

class QXmlResultItems;

class XQueryModel : public QStandardItemModel
{
public:
    XQueryModel( const QString & queryString, const QSet<QString>* textPaths, QObject * parent, const QList<Focus> & focus = QList<Focus>() );

    QString resultSummary() const;

private:
    QString mQuery;
    QList<Focus> mFocus;

    bool query( QStandardItem *parentItem, const QString & filename );
};

#endif // SEARCHQUERYMODEL_H
