#ifndef KEYSTROKEDETECTINGLINEEDIT_H
#define KEYSTROKEDETECTINGLINEEDIT_H

#include <QObject>
#include <QLineEdit>

class KeystrokeDetectingLineEdit : public QLineEdit
{
public:
    explicit KeystrokeDetectingLineEdit(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // KEYSTROKEDETECTINGLINEEDIT_H
