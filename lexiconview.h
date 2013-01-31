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

signals:
    void lexicalEntrySelected(qlonglong lexicalEntryId);

public slots:

};

#endif // LEXICONVIEW_H
