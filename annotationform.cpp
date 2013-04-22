#include "annotationform.h"
#include "ui_annotationform.h"

#include <QtDebug>

#include "text.h"
#include "databaseadapter.h"
#include "annotationtype.h"
#include "annotationmodel.h"

AnnotationForm::AnnotationForm(Text *text, const DatabaseAdapter *dbAdapter, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnnotationForm)
{
    ui->setupUi(this);

    mText = text;
    mDbAdapter = dbAdapter;

    QList<AnnotationType> annotationTypes = mDbAdapter->annotationTypes();
    for(int i=0; i<annotationTypes.count(); i++ )
    {
        ui->comboBox->addItem( annotationTypes.at(i).label() );
    }

    mAnnotationModel = new AnnotationModel(text, annotationTypes.first().label() );
    connect( ui->comboBox, SIGNAL(currentTextChanged(QString)), mAnnotationModel, SLOT(setAnnotationType(QString)) );

    qDebug() << mAnnotationModel->rowCount() << mAnnotationModel->columnCount();

    ui->listView->setModel(mAnnotationModel);
}

AnnotationForm::~AnnotationForm()
{
    delete mAnnotationModel;
    delete ui;
}
