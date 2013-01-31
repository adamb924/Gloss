#include "lexiconview.h"

#include <QtDebug>

LexiconView::LexiconView(QWidget *parent) :
    QTableView(parent)
{
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

void LexiconView::selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected )
{
    emit lexicalEntrySelected( selected.indexes().first().data(Qt::UserRole).toLongLong() );
}
