#ifndef LEXICONVIEW_H
#define LEXICONVIEW_H

#include <QTableView>

class LexiconView : public QTableView
{
    Q_OBJECT
public:
    explicit LexiconView(QWidget *parent = 0);

private:
    void selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected );
    void mouseDoubleClickEvent(QMouseEvent * event);

signals:
    void lexicalEntrySelected(qlonglong lexicalEntryId);
    void requestEditForm(const QModelIndex & index);

public slots:

};

#endif // LEXICONVIEW_H
