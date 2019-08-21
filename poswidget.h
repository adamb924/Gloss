#ifndef POSWIDGET_H
#define POSWIDGET_H

#include <QWidget>

#include "glossitem.h"

namespace Ui {
class PosWidget;
}

class PosWidget : public QWidget
{
    Q_OBJECT

public:
    PosWidget(const GlossItem *glossItem, const QHash<QString, PartOfSpeech> & partsOfSpeech, QWidget *parent = nullptr);
    ~PosWidget();

private:
    Ui::PosWidget *ui;
    const GlossItem *mGlossItem;
};

#endif // POSWIDGET_H
