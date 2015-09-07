#include "annotationeditordialog.h"
#include "ui_annotationeditordialog.h"

AnnotationEditorDialog::AnnotationEditorDialog(const Annotation &annotation, const TextBit &hint, const QString &annotationKey, const QList<AnnotationType>* types, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnnotationEditorDialog),
    mHint(hint)
{
    ui->setupUi(this);
    ui->textEdit->setTextBit( annotation.text() );
    ui->headerEdit->setTextBit( annotation.header() );
    foreach(AnnotationType type, *types)
    {
        ui->annotationType->addItem( type.label() );
    }
    ui->annotationType->setCurrentText( annotationKey );

    connect(ui->guessButton, SIGNAL(clicked()), this, SLOT(guessButton()));
    connect(ui->clearAll, SIGNAL(clicked()), this, SLOT(clearAll()) );
}

AnnotationEditorDialog::~AnnotationEditorDialog()
{
    delete ui;
}

Annotation AnnotationEditorDialog::annotation() const
{
    return Annotation( ui->headerEdit->textBit() , ui->textEdit->textBit() );
}

QString AnnotationEditorDialog::newAnnotationType() const
{
    return ui->annotationType->currentText();
}

void AnnotationEditorDialog::guessButton()
{
    ui->headerEdit->setText( mHint.text() );
}

void AnnotationEditorDialog::clearAll()
{
    ui->textEdit->setText( "" );
    ui->headerEdit->setText( "" );
    accept();
}
