#include "tagview.h"

#include <QContextMenuEvent>
#include <QMenu>
#include <QtDebug>

TagView::TagView(QWidget *parent) :
    QListView(parent)
{
}

void TagView::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);
    menu.addAction(tr("Remove"), this, SLOT(removeCurrentRow()));
    menu.exec(event->globalPos());
}

void TagView::removeCurrentRow()
{
    model()->removeRows( currentIndex().row() , 1 );
}
