#ifndef SEGMENTATIONOPTIONBUTTON_H
#define SEGMENTATIONOPTIONBUTTON_H

#include <QCommandLinkButton>

#include "textbit.h"

class SegmentationOptionButton : public QCommandLinkButton
{
    Q_OBJECT
public:
    SegmentationOptionButton(const TextBit & text, QWidget * parent = 0);

signals:
    void clicked(const TextBit & text);
    void rightClicked(const TextBit & text);

protected:
    void mouseReleaseEvent(QMouseEvent * event);

private slots:
    void emitClicked();

private:
    TextBit mTextBit;
};

#endif // SEGMENTATIONOPTIONBUTTON_H
