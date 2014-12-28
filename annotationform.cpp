#include "annotationform.h"
#include "ui_annotationform.h"

#include <QtDebug>

#include "text.h"
#include "databaseadapter.h"
#include "annotationtype.h"
#include "annotationmodel.h"
#include "project.h"

AnnotationForm::AnnotationForm(Text *text, const Project *project, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnnotationForm),
    mProject(project)
{
    ui->setupUi(this);

    mText = text;
    mDbAdapter = mProject->dbAdapter();

    const QList<AnnotationType> *annotationTypes = mProject->annotationTypes();
    for(int i=0; i<annotationTypes->count(); i++ )
    {
        ui->comboBox->addItem( annotationTypes->at(i).label() );
    }

    mAnnotationModel = new AnnotationModel(text, annotationTypes->first().label() );
    connect( text, SIGNAL(annotationsChanged()), mAnnotationModel, SLOT(loadGlossItems()));
    connect( ui->comboBox, SIGNAL(currentTextChanged(QString)), mAnnotationModel, SLOT(setAnnotationType(QString)) );
    connect( ui->comboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(modelReset()) );
    ui->treeView->setModel(mAnnotationModel);
    ui->treeView->setEditTriggers(QAbstractItemView::SelectedClicked);

    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(focusLine(QModelIndex)) );
}

AnnotationForm::~AnnotationForm()
{
    delete mAnnotationModel;
    delete ui;
}

void AnnotationForm::focusLine(const QModelIndex & index)
{
    int line, position;
    mText->findGlossItemLocation( mAnnotationModel->glossItem(index) , line , position );
    if( line == -1 || position == -1 )
        return;

    line++; // make it 1-indexed

    // TODO focusing the interpretation isn't strictly correct. We actually want to focus the gloss item itself.
    QList<Focus> foci;
    foci << Focus( Focus::GlossItem,  (qlonglong)mAnnotationModel->glossItem(index) );

    emit focusTextPosition( mText->name(), line, foci );
}

void AnnotationForm::modelReset()
{
    ui->treeView->resizeColumnToContents(0);
}
