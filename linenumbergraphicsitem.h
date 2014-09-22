/*!
  \class LineNumberGraphicsItem
  \ingroup SyntaxGUI
  \brief This class provides a graphics item (QGraphicsItem) for displaying the line number in a text. It is used in SyntacticParsingWidget.

  This class is for displaying line numbers in a syntactic analysis view. Line numbers displayed in an interlinear text view are InterlinearLineLabel objects.
*/

#ifndef LINENUMBERGRAPHICSITEM_H
#define LINENUMBERGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QObject>
#include <QFont>

class QGraphicsSceneMouseEvent;

class LineNumberGraphicsItem : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    LineNumberGraphicsItem(int lineNumber);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);

signals:
    void requestPlayLine(int line);

private:
    int mLineNumber;
    QFont mFont;
};

#endif // LINENUMBERGRAPHICSITEM_H
