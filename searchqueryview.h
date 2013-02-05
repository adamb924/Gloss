#ifndef SEARCHQUERYVIEW_H
#define SEARCHQUERYVIEW_H

#include <QTreeView>
#include <QList>

class Focus;

class SearchQueryView : public QTreeView
{
    Q_OBJECT
public:
    explicit SearchQueryView(QWidget *parent = 0);

signals:
    void requestOpenText( const QString & textName , int lineNumber, const QList<Focus> & foci );
    void requestPlaySound( const QString & textName , int lineNumber );
    void requestEditLine( const QString & textName , int lineNumber, const QList<Focus> & foci );
    void requestEditLineWithContext( const QString & textName , int lineNumber, const QList<Focus> & foci );

public slots:
    void contextMenu( const QPoint & pos );

private:
    void getDetails( const QModelIndex & index, QString &textName, int &lineNumber, QList<Focus> & foci ) const;

private slots:
    void openText();
    void playSound();
    void editLine();
    void editLineWithContext();
};

#endif // SEARCHQUERYVIEW_H
