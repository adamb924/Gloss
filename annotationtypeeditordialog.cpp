#include "annotationtypeeditordialog.h"
#include "ui_annotationeditordialog.h"

#include "annotationtype.h"

AnnotationTypeEditorDialog::AnnotationTypeEditorDialog(const QList<WritingSystem> &writingSystems, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnnotationTypeEditorDialog)
{
    ui->setupUi(this);

    ui->writingSystemCombo->setWritingSystems(writingSystems);
}

AnnotationTypeEditorDialog::AnnotationTypeEditorDialog(AnnotationType type, const QList<WritingSystem> &writingSystems, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnnotationTypeEditorDialog)
{
    ui->setupUi(this);

    ui->writingSystemCombo->setWritingSystems(writingSystems);
    ui->writingSystemCombo->setCurrentWritingSystem( type.writingSystem() );

    ui->nameEdit->setText( type.label() );

    ui->markEdit->setText( type.mark() );
}

AnnotationTypeEditorDialog::~AnnotationTypeEditorDialog()
{
    delete ui;
}

AnnotationType AnnotationTypeEditorDialog::annotationType() const
{
    return AnnotationType(ui->nameEdit->text(), ui->markEdit->text(), ui->writingSystemCombo->currentWritingSystem() );
}
