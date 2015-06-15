#include "textmetadatadialog.h"
#include "ui_textmetadatadialog.h"

#include <QDataWidgetMapper>

#include "project.h"
#include "textlistmodel.h"

TextMetadataDialog::TextMetadataDialog(Project *project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextMetadataDialog),
    mProject(project)
{
    ui->setupUi(this);

    TextListModel * model = new TextListModel(mProject);

    ui->listView->setModel( model );
    ui->listView->setModelColumn(0);

    mMapper = new QDataWidgetMapper;
    mMapper->setModel( model );
    mMapper->addMapping( ui->titleEdit , 0 );
    mMapper->addMapping( ui->commentEdit , 1 );
    mMapper->addMapping( ui->exportFilenameEdit , 2 );
    mMapper->toFirst();

    connect( ui->listView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), mMapper, SLOT(setCurrentModelIndex(QModelIndex)) );
    connect( mProject, SIGNAL(textsChanged()), model, SLOT(dataChanged()) );
    connect( mProject, SIGNAL(textsChanged()), ui->listView, SLOT(reset()) );
}

TextMetadataDialog::~TextMetadataDialog()
{
    delete ui;
}
