#ifndef TEXTDISPLAYWIDGET_H
#define TEXTDISPLAYWIDGET_H

#include <QTabWidget>

#include "project.h"

//class Project;
class InterlinearDisplayWidget;

namespace Ui {
    class TextDisplayWidget;
}

class TextDisplayWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit TextDisplayWidget(Project *project, QWidget *parent = 0);
    ~TextDisplayWidget();

    Project::BaselineMode textBaselineMode() const;

    WritingSystem* writingSystem() const;
    void setWritingSystem(WritingSystem *ws);

private:
    Ui::TextDisplayWidget *ui;
    Project *mProject;
    InterlinearDisplayWidget *mInterlinear;
    Project::BaselineMode mBaselineMode;
    WritingSystem *mWritingSystem; // for the baseline text

private slots:
    void tabChanged(int i);

};

#endif // TEXTDISPLAYWIDGET_H
