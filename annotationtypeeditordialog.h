/*!
  \class AnnotationTypeEditorDialog
  \ingroup GUI
  \brief This is a dialog for editing an AnnotationType. It's currently used in ProjectOptionsDialog. The associated UI file is annotationtypeeditordialog.h.
*/

#ifndef ANNOTATIONTYPEEDITORDIALOG_H
#define ANNOTATIONTYPEEDITORDIALOG_H

#include <QDialog>

namespace Ui {
class AnnotationTypeEditorDialog;
}

class AnnotationType;
class WritingSystem;

class AnnotationTypeEditorDialog : public QDialog
{
    Q_OBJECT

public:
    AnnotationTypeEditorDialog( const QList<WritingSystem> & writingSystems, QWidget *parent = nullptr);
    AnnotationTypeEditorDialog( AnnotationType type, const QList<WritingSystem> & writingSystems, QWidget *parent = nullptr);
    ~AnnotationTypeEditorDialog();

    AnnotationType annotationType() const;

private:
    Ui::AnnotationTypeEditorDialog *ui;
};

#endif // ANNOTATIONTYPEEDITORDIALOG_H
