#ifndef INTERLINEARLINELABEL_H
#define INTERLINEARLINELABEL_H

#include <QLabel>

class InterlinearLineLabel : public QLabel
{
    Q_OBJECT
public:
    InterlinearLineLabel(int lineNumber, const QString & label, QWidget *parent = 0);

signals:
    void playSound(int lineNumber);
    void approveAll(int lineNumber);

private:
    void contextMenuEvent ( QContextMenuEvent * event );

    int mLineNumber;

private slots:
    void emitApproveAll();
    void emitPlaySound();

};

#endif // INTERLINEARLINELABEL_H
