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
    ui->listView->setModel(mAnnotationModel);
    ui->listView->setEditTriggers(QAbstractItemView::SelectedClicked);

    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(focusLine(QModelIndex)) );
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
