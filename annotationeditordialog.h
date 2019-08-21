#ifndef ANNOTATIONEDITORDIALOG_H
#define ANNOTATIONEDITORDIALOG_H

#include <QDialog>
#include <QList>

#include "annotation.h"
#include "annotationtype.h"

namespace Ui {
class AnnotationEditorDialog;
}

class AnnotationEditorDialog : public QDialog
{
    Q_OBJECT

public:
    AnnotationEditorDialog(const Annotation & annotation, const TextBit & hint, const QString & annotationKey, const QList<AnnotationType>* types, QWidget *parent = nullptr);
    ~AnnotationEditorDialog();

    Annotation annotation() const;
    QString newAnnotationType() const;

private slots:
    void guessButton();
    void clearAll();

private:
    Ui::AnnotationEditorDialog *ui;
    TextBit mHint;
};

#endif // ANNOTATIONEDITORDIALOG_H
