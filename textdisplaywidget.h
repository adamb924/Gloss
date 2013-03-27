/*!
  \class TextDisplayWidget
  \ingroup GUI
  \brief A QTabWidget subclass, providing three tabs for various interfaces to a text: Baseline, Glossing, and Morphological Analysis. The layout is textdisplaywidget.ui.
*/

#ifndef TEXTDISPLAYWIDGET_H
#define TEXTDISPLAYWIDGET_H

#include <QTabWidget>

#include "project.h"
#include "text.h"

class InterlinearDisplayWidget;
class LingTextEdit;

#include "focus.h"
#include "view.h"

namespace Ui {
    class TextDisplayWidget;
}

class TextDisplayWidget : public QTabWidget
{
    Q_OBJECT

public:
    TextDisplayWidget(Text *text, Project *project, View::Type type, const QList<int> & lines, const QList<Focus> & foci = QList<Focus>(), QWidget *parent = 0);
    ~TextDisplayWidget();

    void focusGlossLine(int line);
    void setLines(const QList<int> & lines);
    void saveText();

private:
    Ui::TextDisplayWidget *ui;
    Project *mProject;
    Text *mText;
    LingTextEdit * mBaselineTextEdit;
    QList<InterlinearDisplayWidget*> mIdwTabs;

    void setupBaselineTab();


protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void tabChanged(int i);

};

#endif // TEXTDISPLAYWIDGET_H
