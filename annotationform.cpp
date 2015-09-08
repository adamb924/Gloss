#include "annotationform.h"
#include "ui_annotationform.h"

#include <QtDebug>

#include "text.h"
#include "databaseadapter.h"
#include "annotationtype.h"
#include "annotationmodel.h"
#include "project.h"
#include "annotationeditordialog.h"

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
    ui->treeView->resizeColumnToContents(0);

    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClick(QModelIndex)) );
}

AnnotationForm::~AnnotationForm()
{
    delete mAnnotationModel;
    delete ui;
}

void AnnotationForm::doubleClick(const QModelIndex & index)
{
    if( index.column() == 0 ) // double-clicking the line number should jump to the line
    {
        int phrase = mText->lineNumberForGlossItem( mAnnotationModel->glossItem(index) );
        phrase++; // make it 1-indexed

        QList<Focus> foci;
        foci << Focus( Focus::GlossItem,  (qlonglong)mAnnotationModel->glossItem(index) );

        emit focusTextPosition( mText->name(), phrase, foci );
    }
    else // double-clicking anywhere else launches the annotation editor
    {
        GlossItem * glossItem = mAnnotationModel->glossItem(index);
        QString key = ui->comboBox->currentText();
        Annotation annotation = glossItem->getAnnotation( key );
        annotation.setWritingSystem( mProject->annotationType(key).writingSystem() );
        annotation.setHeaderWritingSystem( mProject->annotationType(key).headerWritingSystem() );

        AnnotationEditorDialog dialog( annotation, glossItem->baselineText(), key, mProject->annotationTypes() );
        dialog.setWindowTitle(key);
        if( dialog.exec() == QDialog::Accepted )
        {
            if( dialog.newAnnotationType() == key )
            {
                glossItem->setAnnotation( key , dialog.annotation() );
            }
            else
            {
                glossItem->setAnnotation( dialog.newAnnotationType(), dialog.annotation() );
                glossItem->setAnnotation( key , Annotation() );
            }
        }
    }
}

void AnnotationForm::modelReset()
{
    ui->treeView->resizeColumnToContents(0);
}
