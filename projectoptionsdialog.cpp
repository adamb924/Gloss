#include "projectoptionsdialog.h"
#include "ui_projectoptionsdialog.h"
#include "project.h"
#include "databaseadapter.h"
#include "writingsystemlistmodel.h"
#include "writingsystemdialog.h"
#include "annotationeditordialog.h"
#include "annotationtypelistmodel.h"

#include <QFileDialog>
#include <QtDebug>

ProjectOptionsDialog::ProjectOptionsDialog(Project * project, QWidget *parent) :
    mProject(project),
    QDialog(parent),
    ui(new Ui::ProjectOptionsDialog)
{
    ui->setupUi(this);

    ui->metaWritingSystemCombo->setWritingSystems( mProject->dbAdapter()->writingSystems() );
    ui->metaWritingSystemCombo->setCurrentWritingSystem( mProject->metaLanguage() );

    ui->defaultGlossWS->setWritingSystems( mProject->dbAdapter()->writingSystems() );
    ui->defaultGlossWS->setCurrentWritingSystem( mProject->defaultGlossLanguage() );

    ui->defaultTextFormWS->setWritingSystems( mProject->dbAdapter()->writingSystems() );
    ui->defaultTextFormWS->setCurrentWritingSystem( mProject->defaultTextFormLanguage() );

    ui->mediaFolder->setText( mProject->mediaFolder() );

    mAnnotationTypeModel = new AnnotationTypeListModel( mProject->annotationTypes() );
    ui->interlinearAnnotationsList->setModel(mAnnotationTypeModel);

    mCitationModel = new WritingSystemListModel( mProject->lexicalEntryCitationFormFields() );
    ui->leCitationFormsList->setModel(mCitationModel);

    mGlossModel = new WritingSystemListModel( mProject->lexicalEntryGlossFields() );
    ui->leGlossesList->setModel(mGlossModel);

    connect( ui->mediaFolderChooser, SIGNAL(clicked()), this, SLOT(mediaFolderChooser()) );

    connect( ui->addAnnotation, SIGNAL(clicked()), this, SLOT(addAnnotation()) );
    connect( ui->removeAnnotation, SIGNAL(clicked()), this, SLOT(removeAnnotation()) );
    connect( ui->interlinearAnnotationsList, SIGNAL(activated(QModelIndex)), this, SLOT(editAnnotation(QModelIndex)) );

    connect( ui->addLECitation, SIGNAL(clicked()), this, SLOT(addCitation()) );
    connect( ui->removeLECitation, SIGNAL(clicked()), this, SLOT(removeCitation()) );
    connect( ui->leCitationFormsList, SIGNAL(activated(QModelIndex)), this, SLOT(editCitation(QModelIndex)) );

    connect( ui->addLEGloss, SIGNAL(clicked()), this, SLOT(addGloss()) );
    connect( ui->removeLEGloss, SIGNAL(clicked()), this, SLOT(removeGloss()) );
    connect( ui->leGlossesList, SIGNAL(activated(QModelIndex)), this, SLOT(editGloss(QModelIndex)) );
}

ProjectOptionsDialog::~ProjectOptionsDialog()
{
    delete ui;
}

void ProjectOptionsDialog::mediaFolderChooser()
{
    QFileDialog dlg;
    dlg.setFileMode(QFileDialog::Directory);
    if(dlg.exec())
    {
        ui->mediaFolder->setText( dlg.directory().path() );
    }
}

void ProjectOptionsDialog::addAnnotation()
{
    AnnotationEditorDialog dialog(mProject->dbAdapter()->writingSystems());
    if( dialog.exec() )
    {
        mAnnotationTypeModel->addAnnotationType(dialog.annotationType());
    }
}

void ProjectOptionsDialog::removeAnnotation()
{
    if( ui->interlinearAnnotationsList->selectionModel()->selectedRows().count() > 0 )
    {
        mAnnotationTypeModel->removeAnnotationType( ui->interlinearAnnotationsList->selectionModel()->selectedRows().first() );
    }
}

void ProjectOptionsDialog::editAnnotation(const QModelIndex &index)
{
    AnnotationEditorDialog dialog( mProject->annotationTypes()->at(index.row()), mProject->dbAdapter()->writingSystems());
    if( dialog.exec() )
    {
        mAnnotationTypeModel->setAnnotationType( index, dialog.annotationType());
    }
}

void ProjectOptionsDialog::addCitation()
{
    WritingSystemDialog dialog(mProject->dbAdapter()->writingSystems());
    if( dialog.exec() )
    {
       mCitationModel->addWritingSystem( dialog.writingSystem() );
    }
}

void ProjectOptionsDialog::removeCitation()
{
    if( ui->leCitationFormsList->selectionModel()->selectedRows().count() > 0 )
    {
        mCitationModel->removeWritingSystem( ui->leCitationFormsList->selectionModel()->selectedRows().first() );
    }
}

void ProjectOptionsDialog::editCitation(const QModelIndex &index)
{
    WritingSystemDialog dialog(mProject->dbAdapter()->writingSystems());
    dialog.setCurrentWritingSystem( mProject->lexicalEntryCitationFormFields()->at(index.row()) );
    if( dialog.exec() )
    {
       mCitationModel->setWritingSystem(index, dialog.writingSystem() );
    }
}

void ProjectOptionsDialog::addGloss()
{
    WritingSystemDialog dialog(mProject->dbAdapter()->writingSystems());
    if( dialog.exec() )
    {
       mGlossModel->addWritingSystem( dialog.writingSystem() );
    }
}

void ProjectOptionsDialog::removeGloss()
{
    if( ui->leGlossesList->selectionModel()->selectedRows().count() > 0 )
    {
        mGlossModel->removeWritingSystem( ui->leGlossesList->selectionModel()->selectedRows().first() );
    }
}

void ProjectOptionsDialog::editGloss(const QModelIndex &index)
{
    WritingSystemDialog dialog(mProject->dbAdapter()->writingSystems());
    dialog.setCurrentWritingSystem( mProject->lexicalEntryGlossFields()->at(index.row()) );
    if( dialog.exec() )
    {
       mGlossModel->setWritingSystem(index, dialog.writingSystem() );
    }
}
