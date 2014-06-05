/*!
  \class AnnotationEditorDialog
  \ingroup GUI
  \brief This is a dialog for editing an AnnotationType. It's currently used in ProjectOptionsDialog. The associated UI file is annotationeditordialog.h.
*/

#ifndef ANNOTATIONEDITORDIALOG_H
#define ANNOTATIONEDITORDIALOG_H

#include <QDialog>

namespace Ui {
class AnnotationEditorDialog;
}

class AnnotationType;
class WritingSystem;

class AnnotationEditorDialog : public QDialog
{
    Q_OBJECT

public:
    AnnotationEditorDialog( const QList<WritingSystem> & writingSystems, QWidget *parent = 0);
    AnnotationEditorDialog( AnnotationType type, const QList<WritingSystem> & writingSystems, QWidget *parent = 0);
    ~AnnotationEditorDialog();

    AnnotationType annotationType() const;

private:
    Ui::AnnotationEditorDialog *ui;
};

#endif // ANNOTATIONEDITORDIALOG_H
