#ifndef TAGVIEW_H
#define TAGVIEW_H

#include <QListView>

class TagView : public QListView
{
    Q_OBJECT
public:
    explicit TagView(QWidget *parent = 0);

signals:

public slots:

private:
    void contextMenuEvent ( QContextMenuEvent * event );

private slots:
    void removeCurrentRow();
};

#endif // TAGVIEW_H
