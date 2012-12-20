#include "mergetranslationdialog.h"
#include "ui_mergetranslationdialog.h"

#include <QFileDialog>

#include "databaseadapter.h"
#include "writingsystem.h"
#include "project.h"

MergeTranslationDialog::MergeTranslationDialog(Project *project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MergeTranslationDialog)
{
    ui->setupUi(this);

    mProject = project;
    mDbAdapter = mProject->dbAdapter();
    mWritingSystems = mDbAdapter->writingSystems();

    // Writing systems
    for(int i=0; i<mWritingSystems.count(); i++)
        ui->writingSystemCombo->addItem(mWritingSystems.at(i).summaryString(), mWritingSystems.at(i).flexString());

    QStringList texts = mProject->textNames();
    for(int i=0; i<texts.count(); i++)
        ui->textCombo->addItem( texts.at(i) );

    connect(ui->choose, SIGNAL(clicked()), this, SLOT(chooseFile()));
    connect(ui->textCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(checkParity()));
    connect(ui->filename, SIGNAL(textChanged(QString)), this, SLOT(checkParity()));
}

MergeTranslationDialog::~MergeTranslationDialog()
{
    delete ui;
}

void MergeTranslationDialog::checkParity()
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

QString MergeTranslationDialog::text() const
{
    return ui->textCombo->currentText();
}

WritingSystem MergeTranslationDialog::writingSystem() const
{
    return mWritingSystems.at(ui->writingSystemCombo->currentIndex());
}

QString MergeTranslationDialog::filename() const
{
    return ui->filename->text();
}

void MergeTranslationDialog::chooseFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open") , QString(), "FlexText (*.flextext)" );
    if( !filename.isNull() )
        ui->filename->setText( filename );
}
