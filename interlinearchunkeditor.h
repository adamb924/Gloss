#ifndef INTERLINEARCHUNKEDITOR_H
#define INTERLINEARCHUNKEDITOR_H

#include <QWidget>

class Text;
class Project;
class TextDisplayWidget;

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

private:
    Ui::InterlinearChunkEditor *ui;

    Text * mText;
    Project * mProject;
    View::Type mType;
    TextDisplayWidget * mTextDisplayWidget;

    int mChunkSize;
    int mPosition;
    int mNPhrases;

    void moveToPosition(int position);
    void setButtonActivation();
    QList<int> makeLines();

    void closeEvent(QCloseEvent *event);

private slots:
    void next();
    void previous();
    void goTo();
};

#endif // INTERLINEARCHUNKEDITOR_H
