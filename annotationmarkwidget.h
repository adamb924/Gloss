#ifndef ANNOTATIONMARKWIDGET_H
#define ANNOTATIONMARKWIDGET_H

#include <QtWidgets>
#include <QList>
#include "annotationtype.h"
class GlossItem;

class Mark : public QLabel
{
    Q_OBJECT
public:
    Mark(const QString & text, QWidget *parent) : QLabel(text, parent) {}
signals:
    void doubleClicked( Mark * mark );
protected:
    void mouseDoubleClickEvent(QMouseEvent * event)
    {
        emit doubleClicked( this );
    }
};

class AnnotationMarkWidget : public QWidget
{
    Q_OBJECT
public:
    AnnotationMarkWidget(const QList<AnnotationType> & annotationTypes, GlossItem * glossItem, QWidget *parent = 0);

signals:
    void annotationActivated( GlossItem * item , const QString & string );
    void annotationActivated( const QString & string );

public slots:
    void setupLayout();

private slots:
    void markActivated( Mark * mark );

private:
    GlossItem * mGlossItem;
    QList<AnnotationType> mAnnotationTypes;
    QVBoxLayout * mLayout;
    QList<Mark*> mMarks;
};

#endif // ANNOTATIONMARKWIDGET_H
