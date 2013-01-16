#ifndef SEARCHQUERYVIEW_H
#define SEARCHQUERYVIEW_H

#include <QTreeView>

class SearchQueryView : public QTreeView
{
    Q_OBJECT
public:
    explicit SearchQueryView(QWidget *parent = 0);

signals:
    void requestOpenText( const QString & textName , int lineNumber );
    void requestPlaySound( const QString & textName , int lineNumber );
    void requestEditLine( const QString & textName , int lineNumber );
    void requestEditLineWithContext( const QString & textName , int lineNumber );

public slots:
    void contextMenu( const QPoint & pos );

private:
    void getDetails( const QModelIndex & index, QString &textName, int &lineNumber ) const;

private slots:
    void openText();
    void playSound();
    void editLine();
    void editLineWithContext();
};

#endif // SEARCHQUERYVIEW_H
