#ifndef INTERLINEARLINELABEL_H
#define INTERLINEARLINELABEL_H

#include <QLabel>

class InterlinearLineLabel : public QLabel
{
    Q_OBJECT
public:
    InterlinearLineLabel(int lineNumber, const QString & label, bool soundAvailable, const QString & soundSummary, QWidget *parent = 0);

signals:
    void playSound(int lineNumber);
    void approveAll(int lineNumber);
    void editLine(int lineNumber);

private:
    void contextMenuEvent ( QContextMenuEvent * event );
    void mouseDoubleClickEvent ( QMouseEvent * event );

    int mLineNumber;
    bool mSoundAvailable;

private slots:
    void emitApproveAll();
    void emitPlaySound();
    void emitEditLine();

};

#endif // INTERLINEARLINELABEL_H
