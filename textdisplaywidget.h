#ifndef TEXTDISPLAYWIDGET_H
#define TEXTDISPLAYWIDGET_H

#include <QTabWidget>

#include "project.h"
#include "text.h"

class InterlinearDisplayWidget;

namespace Ui {
    class TextDisplayWidget;
}

class TextDisplayWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit TextDisplayWidget(Text *text, Project *project, QWidget *parent = 0);
    ~TextDisplayWidget();

private:
    Ui::TextDisplayWidget *ui;
    Project *mProject;
    InterlinearDisplayWidget *mInterlinear;
    Text *mText;

private slots:
    void tabChanged(int i);

};

#endif // TEXTDISPLAYWIDGET_H
