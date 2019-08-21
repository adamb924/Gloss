#ifndef ANNOTATIONMARKWIDGET_H
#define ANNOTATIONMARKWIDGET_H

#include <QtWidgets>
#include <QList>
#include "annotationtype.h"
class GlossItem;

/*!
  \class Mark
  \ingroup Interlinear
  \brief This is subclassed QLabel, which emits a signal when it is double-clicked. It is used in AnnotationMarkWidget.
*/

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
        Q_UNUSED(event);
        emit doubleClicked( this );
    }
};

/*!
  \class AnnotationMarkWidget
  \ingroup Interlinear
  \brief This GUI widget contains one or more Mark objects. It receives signals from the Mark objects and emits signals indicating which mark was activated, and for which GlossItem.
*/

class AnnotationMarkWidget : public QWidget
{
    Q_OBJECT
public:
    AnnotationMarkWidget(const QList<AnnotationType> * annotationTypes, const GlossItem * glossItem, QWidget *parent = nullptr);

signals:
    void annotationActivated( const QString & string );

public slots:
    void setupLayout();

private slots:
    void markActivated( Mark * mark );

private:
    const GlossItem * mGlossItem;
    const QList<AnnotationType> * mAnnotationTypes;
    QVBoxLayout * mLayout;
    QList<Mark*> mMarks;
};

#endif // ANNOTATIONMARKWIDGET_H
