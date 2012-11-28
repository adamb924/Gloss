#ifndef TEXTDISPLAYWIDGET_H
#define TEXTDISPLAYWIDGET_H

#include <QTabWidget>

#include "project.h"
#include "textinfo.h"

class InterlinearDisplayWidget;

namespace Ui {
    class TextDisplayWidget;
}

class TextDisplayWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit TextDisplayWidget(const TextInfo & info, Project *project, QWidget *parent = 0);
    ~TextDisplayWidget();

    Project::BaselineMode textBaselineMode() const;

private:
    Ui::TextDisplayWidget *ui;
    Project *mProject;
    InterlinearDisplayWidget *mInterlinear;
    TextInfo mTextInfo;

private slots:
    void tabChanged(int i);

};

#endif // TEXTDISPLAYWIDGET_H
