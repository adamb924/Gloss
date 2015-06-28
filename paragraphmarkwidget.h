#ifndef PARAGRAPHMARKWIDGET_H
#define PARAGRAPHMARKWIDGET_H

#include <QWidget>

#include "textbit.h"

class ImmutableLabel;

class ParagraphMarkWidget : public QWidget
{
    Q_OBJECT
public:
    ParagraphMarkWidget(const TextBit & header, QWidget *parent = 0);

signals:
    void headerChanged(const TextBit & newHeader);

private slots:
    void editHeader();

private:
    void mouseDoubleClickEvent ( QMouseEvent * event );

    TextBit mHeader;
    ImmutableLabel * mHeaderLabel;
};

#endif // PARAGRAPHMARKWIDGET_H
