#ifndef ANNOTATIONEDITORDIALOG_H
#define ANNOTATIONEDITORDIALOG_H

#include <QDialog>

#include "annotation.h"

namespace Ui {
class AnnotationEditorDialog;
}

class AnnotationEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnnotationEditorDialog(const Annotation & annotation, const TextBit & hint, QWidget *parent = 0);
    ~AnnotationEditorDialog();

    Annotation annotation() const;

private slots:
    void guessButton();

private:
    Ui::AnnotationEditorDialog *ui;
    TextBit mHint;
};

#endif // ANNOTATIONEDITORDIALOG_H
