#ifndef ANNOTATIONFORM_H
#define ANNOTATIONFORM_H

#include <QWidget>

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

private:
    Ui::AnnotationForm *ui;
    Text * mText;
    const DatabaseAdapter * mDbAdapter;
    AnnotationModel * mAnnotationModel;
};

#endif // ANNOTATIONFORM_H
