/*!
  \class TextTabWidget
      \ingroup Interlinear
  \brief A QTabWidget subclass, containing various tabs of type InterlinearDisplayWidget. This can be instantiated for an QMdiSubWindow in MainWindow, or within InterlinearChunkEditor.
*/

#ifndef TEXTTABWIDGET_H
#define TEXTTABWIDGET_H

#include <QTabWidget>

#include "project.h"
#include "text.h"

class InterlinearDisplayWidget;

#include "focus.h"
#include "view.h"

class TextTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    TextTabWidget(Text *text, Project *project, View::Type type, const QList<int> & lines, const QList<Focus> & foci = QList<Focus>(), QWidget *parent = nullptr);
    ~TextTabWidget();

    void setLines(const QList<int> & lines);
    void saveText();
    Text * text();
    void setFocus( const QList<Focus> & foci );

private:
    Project *mProject;
    Text *mText;
    QList<InterlinearDisplayWidget*> mIdwTabs;

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void updatePhrasalGloss( int lineNumber, const TextBit & bit);
};

#endif // TEXTTABWIDGET_H
