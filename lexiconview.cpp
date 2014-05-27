#include "lexiconview.h"

#include <QtDebug>
#include <QMouseEvent>

LexiconView::LexiconView(QWidget *parent) :
    QTableView(parent)
{
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

void LexiconView::selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected )
{
    Q_UNUSED(deselected);
    if( selected.indexes().count() > 0 )
        emit lexicalEntrySelected( selected.indexes().first().data(Qt::UserRole).toLongLong() );
}

void LexiconView::mouseDoubleClickEvent(QMouseEvent * event)
{
    if( event->button() == Qt::RightButton )
    {
        if( selectedIndexes().count() > 0 )
            emit requestEditForm( selectedIndexes().first() );
    }
    else
    {
        QTableView::mouseDoubleClickEvent(event);
    }
}
