/*!
  \class MorphemeGraphicsItem
  \ingroup SyntaxGUI
  \brief This class provides a graphics item (QGraphicsItem) for displaying a morpheme. It is used in SyntacticParsingWidget.
*/

#ifndef MORPHEMEGRAPHICSITEM_H
#define MORPHEMEGRAPHICSITEM_H

#include <QGraphicsSimpleTextItem>

class TextBit;
class Allomorph;
class SyntacticAnalysisElement;

class MorphemeGraphicsItem : public QGraphicsSimpleTextItem
{
public:
    MorphemeGraphicsItem(const TextBit &bit, SyntacticAnalysisElement * element, QGraphicsItem *parent = nullptr);

    SyntacticAnalysisElement* element();

    enum { Type = UserType + 2 };
    int type() const;

private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    SyntacticAnalysisElement * mElement;
};

#endif // MORPHEMEGRAPHICSITEM_H
