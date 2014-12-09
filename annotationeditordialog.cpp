#include "annotationeditordialog.h"
#include "ui_annotationeditordialog.h"

AnnotationEditorDialog::AnnotationEditorDialog(const Annotation &annotation, const TextBit &hint, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnnotationEditorDialog),
    mHint(hint)
{
    ui->setupUi(this);
    ui->annotationEdit->setTextBit( annotation.annotation() );
    ui->headerEdit->setTextBit( annotation.header() );

    connect(ui->guessButton, SIGNAL(clicked()), this, SLOT(guessButton()));
}

AnnotationEditorDialog::~AnnotationEditorDialog()
{
    delete ui;
}

Annotation AnnotationEditorDialog::annotation() const
{
    return Annotation( ui->headerEdit->textBit() , ui->annotationEdit->textBit() );
}

void AnnotationEditorDialog::guessButton()
{
    ui->headerEdit->setText( mHint.text() );
}
