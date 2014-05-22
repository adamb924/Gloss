#ifndef ANNOTATIONFORM_H
#define ANNOTATIONFORM_H

#include <QWidget>
#include "focus.h"

class Text;
class DatabaseAdapter;
class AnnotationModel;

namespace Ui {
class AnnotationForm;
}

class AnnotationForm : public QWidget
{
    Q_OBJECT

public:
    AnnotationForm(Text * text, const DatabaseAdapter * dbAdapter, QWidget *parent = 0);
    ~AnnotationForm();

signals:
    void focusTextPosition( const QString & textName , int lineNumber, const QList<Focus> & foci );

private:
    Ui::AnnotationForm *ui;
    Text * mText;
    const DatabaseAdapter * mDbAdapter;
    AnnotationModel * mAnnotationModel;

private slots:
    void focusLine(const QModelIndex & index);
};

#endif // ANNOTATIONFORM_H
