#ifndef SYNTACTICPARSINGWIDGET_H
#define SYNTACTICPARSINGWIDGET_H

#include <QWidget>

namespace Ui {
class SyntacticParsingWidget;
}

class SyntacticParsingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SyntacticParsingWidget(QWidget *parent = 0);
    ~SyntacticParsingWidget();

private:
    Ui::SyntacticParsingWidget *ui;
};

#endif // SYNTACTICPARSINGWIDGET_H
