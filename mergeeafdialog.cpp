#include "mergeeafdialog.h"
#include "ui_mergeeafdialog.h"

#include "project.h"

#include <QFileDialog>

MergeEafDialog::MergeEafDialog(Project *project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MergeEafDialog)
{
    ui->setupUi(this);

    QStringList texts = project->textNames();
    for(int i=0; i<texts.count(); i++)
        ui->textCombo->addItem( texts.at(i) );

    connect(ui->choose, SIGNAL(clicked()), this, SLOT(chooseFile()));
    connect(ui->textCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(checkParity()));
    connect(ui->filename, SIGNAL(textChanged(QString)), this, SLOT(checkParity()));
}

MergeEafDialog::~MergeEafDialog()
{
    delete ui;
}

QString MergeEafDialog::text() const
{
    return ui->textCombo->currentText();
}

QString MergeEafDialog::filename() const
{
    return ui->filename->text();
}

void MergeEafDialog::chooseFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open") , QString(), "Elan Files (*.eaf)" );
    if( !filename.isNull() )
        ui->filename->setText( filename );
}

void MergeEafDialog::checkParity()
{
    QFileInfo info( ui->filename->text() );
    if( info.baseName() == ui->textCombo->currentText() )
    {
        ui->matchingLabel->setText(tr("The filename and text name match."));
        ui->matchingLabel->setStyleSheet( "color: green;" );
    }
    else
    {
        ui->matchingLabel->setText(tr("The filename and text name do not match. (This is not necessarily wrong.)"));
        ui->matchingLabel->setStyleSheet( "color: red;");
    }
}
