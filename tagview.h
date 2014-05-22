/*!
  \class TagView
  \ingroup GUI
  \ingroup LexiconEdit
  \brief This class provides a view for tags associated with a lexical item. It is to be used with TagModel. The subclass is necessary to get a context menu. It is instantiated in LexiconEdit (only in lexiconedit.ui).
*/

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
