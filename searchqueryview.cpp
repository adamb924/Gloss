#include "searchqueryview.h"

#include <QMenu>
#include <QtDebug>

SearchQueryView::SearchQueryView(QWidget *parent) :
    QTreeView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)) );
    connect( this, SIGNAL(activated(QModelIndex)), this, SLOT(editLine()) );
}

void SearchQueryView::contextMenu( const QPoint & pos )
{
    QMenu menu(this);
    menu.addAction(tr("Edit line"), this, SLOT(editLine()));
    menu.addAction(tr("Edit line with context"), this, SLOT(editLineWithContext()));
    menu.addAction(tr("Open text"), this, SLOT(openText()));
    menu.addAction(tr("Play sound"), this, SLOT(playSound()));
    menu.exec( mapToGlobal(pos) );
}

void SearchQueryView::getDetails( const QModelIndex & index, QString &textName, int &lineNumber ) const
{
    lineNumber= index.data(Qt::UserRole).toInt();
    textName = index.parent().data(Qt::UserRole).toString();
}

void SearchQueryView::openText()
{
    QString textName;
    int lineNumber;
    getDetails( currentIndex(), textName , lineNumber );
    emit requestOpenText( textName, lineNumber );
}

void SearchQueryView::playSound()
{
    QString textName;
    int lineNumber;
    getDetails( currentIndex(), textName , lineNumber );
    emit requestPlaySound( textName, lineNumber );
}

void SearchQueryView::editLine()
{
    QString textName;
    int lineNumber;
    getDetails( currentIndex(), textName , lineNumber );
    emit requestEditLine( textName, lineNumber );
}

void SearchQueryView::editLineWithContext()
{
    QString textName;
    int lineNumber;
    getDetails( currentIndex(), textName , lineNumber );
    emit requestEditLineWithContext( textName, lineNumber );
}
