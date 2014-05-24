#include "annotationeditordialog.h"
#include "ui_annotationeditordialog.h"

#include "annotationtype.h"

AnnotationEditorDialog::AnnotationEditorDialog(const QList<WritingSystem> &writingSystems, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnnotationEditorDialog)
{
    ui->setupUi(this);

    ui->writingSystemCombo->setWritingSystems(writingSystems);
}

AnnotationEditorDialog::AnnotationEditorDialog(AnnotationType type, const QList<WritingSystem> &writingSystems, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnnotationEditorDialog)
{
    ui->setupUi(this);

    ui->writingSystemCombo->setWritingSystems(writingSystems);
    ui->writingSystemCombo->setCurrentWritingSystem( type.writingSystem() );

    ui->nameEdit->setText( type.label() );

    ui->markEdit->setText( type.mark() );
}

AnnotationEditorDialog::~AnnotationEditorDialog()
{
    delete ui;
}

AnnotationType AnnotationEditorDialog::annotationType() const
{
    return AnnotationType(ui->nameEdit->text(), ui->markEdit->text(), ui->writingSystemCombo->currentWritingSystem() );
}
