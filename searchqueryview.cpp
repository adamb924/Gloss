#include "searchqueryview.h"

#include <QMenu>
#include <QtDebug>
#include <QList>

#include "focus.h"

SearchQueryView::SearchQueryView(QWidget *parent) :
    QTreeView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)) );
    connect( this, SIGNAL(activated(QModelIndex)), this, SLOT(editLine()) );
}

void SearchQueryView::contextMenu( const QPoint & pos )
{
    bool isNumeric;
    indexAt(pos).data(Qt::UserRole).toInt(&isNumeric);
    if( isNumeric )
    {
        QMenu menu(this);
        menu.addAction(tr("Edit line"), this, SLOT(editLine()));
        menu.addAction(tr("Open text"), this, SLOT(openText()));
        menu.addAction(tr("Play sound"), this, SLOT(playSound()));
        menu.exec( mapToGlobal(pos) );
    }
}

void SearchQueryView::getDetails( const QModelIndex & index, QString &textName, int &lineNumber, QList<Focus> & foci ) const
{
    bool ok;
    lineNumber= index.data(Qt::UserRole).toInt(&ok);
    if( !ok )
        lineNumber = -1;

    textName = index.parent().data(Qt::UserRole).toString();
    if( textName.isEmpty() )
        textName = index.data(Qt::UserRole).toString();

    QList<QVariant> types = index.data(Focus::TypeList).toList();
    QList<QVariant> indices = index.data(Focus::IndexList).toList();
    for( int i=0; i<types.count(); i++ )
    {
        Focus::Type t = static_cast<Focus::Type>(types.at(i).toInt());
        if( t != Focus::WholeStringSearch && t != Focus::SubStringSearch )
        {
            foci << Focus( t , indices.at(i).toLongLong() );
        }
        else
        {
            foci << Focus( t , indices.at(i).toString() );
        }
    }
}

void SearchQueryView::openText()
{
    QString textName;
    int lineNumber;
    QList<Focus> foci;
    getDetails( currentIndex(), textName , lineNumber, foci );
    emit requestOpenText( textName, lineNumber, foci );
}

void SearchQueryView::playSound()
{
    QString textName;
    int lineNumber;
    QList<Focus> foci;
    getDetails( currentIndex(), textName , lineNumber, foci );
    emit requestPlaySound( textName, lineNumber );
}

void SearchQueryView::editLine()
{
    QString textName;
    int lineNumber;
    QList<Focus> foci;
    getDetails( currentIndex(), textName , lineNumber, foci );
    emit requestEditLine( textName, lineNumber, foci );
}
