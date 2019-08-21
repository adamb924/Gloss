/*!
  \class InterlinearChunkEditor
  \ingroup Interlinear
  \brief This in a widget for displaying a portion of an interlinear text, allowing the user to view a few lines at a time (which is helpful for efficiency).

  This class keeps track of which lines are being displayed. It contains an instance of TextTabWidget, and updates this as needed. (It contains a TextTabWidget instead of an InterlinearDisplayWidget, because users may have several tabs to choose from, displayed a few lines at a time.)
*/

#ifndef INTERLINEARCHUNKEDITOR_H
#define INTERLINEARCHUNKEDITOR_H

#include <QWidget>

class Text;
class Project;
class TextTabWidget;
class Focus;

#include "view.h"

namespace Ui {
class InterlinearChunkEditor;
}

class InterlinearChunkEditor : public QWidget
{
    Q_OBJECT

public:
    InterlinearChunkEditor(Text *text, Project *project, View::Type type, int chunkSize, QWidget *parent = nullptr);
    ~InterlinearChunkEditor();

    void moveToLine(int line);
    Text * text();
    void setFocus( const QList<Focus> & foci );

private:
    Ui::InterlinearChunkEditor *ui;

    Text * mText;
    Project * mProject;
    View::Type mType;
    TextTabWidget * mTextTabWidget;

    int mChunkSize;
    int mPosition;

    void moveToPosition(int position);
    QList<int> makeLines();

    void closeEvent(QCloseEvent *event);

    void keyReleaseEvent(QKeyEvent * event);

private slots:
    void next();
    void previous();
    void goTo();
    void beginning();
    void end();
    void refreshLayout();
};

#endif // INTERLINEARCHUNKEDITOR_H
