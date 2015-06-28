#ifndef PARAGRAPHMARKWIDGET_H
#define PARAGRAPHMARKWIDGET_H

#include <QWidget>

class ImmutableLabel;
class Paragraph;

class ParagraphMarkWidget : public QWidget
{
    Q_OBJECT
public:
    ParagraphMarkWidget(Paragraph *paragraph, QWidget *parent = 0);

signals:
    void removeParagraphDivision(Paragraph * paragraph);

private slots:
    void editHeader();
    void removeParagraphDivision();

private:
    void mouseDoubleClickEvent ( QMouseEvent * event );
    void contextMenuEvent ( QContextMenuEvent * event );

    Paragraph * mParagraph;
    ImmutableLabel * mHeaderLabel;
};

#endif // PARAGRAPHMARKWIDGET_H
