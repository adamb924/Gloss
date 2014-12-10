/*!
  \class InterlinearChunkEditor
  \ingroup Interlinear
  \brief This in a widget for displaying a portion of an interlinear text, allowing the user to view a few lines at a time (which is helpful for efficiency). This is the lightweight version of TextDisplayWidget.
*/

#ifndef INTERLINEARCHUNKEDITOR_H
#define INTERLINEARCHUNKEDITOR_H

#include <QWidget>

class Text;
class Project;
class TextDisplayWidget;
class Focus;

#include "view.h"

namespace Ui {
class InterlinearChunkEditor;
}

class InterlinearChunkEditor : public QWidget
{
    Q_OBJECT

public:
    InterlinearChunkEditor(Text *text, Project *project, View::Type type, int chunkSize, QWidget *parent = 0);
    ~InterlinearChunkEditor();

    void moveToLine(int line);
    Text * text();
    void setFocus( const QList<Focus> & foci );

private:
    Ui::InterlinearChunkEditor *ui;

    Text * mText;
    Project * mProject;
    View::Type mType;
    TextDisplayWidget * mTextDisplayWidget;

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
    void setButtonActivation();
};

#endif // INTERLINEARCHUNKEDITOR_H
