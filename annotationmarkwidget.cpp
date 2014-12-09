#include "annotationmarkwidget.h"
#include "annotationtype.h"
#include "textbit.h"
#include "glossitem.h"

AnnotationMarkWidget::AnnotationMarkWidget(const QList<AnnotationType> *annotationTypes, const GlossItem *glossItem, QWidget *parent) :
    QWidget(parent),
    mGlossItem(glossItem),
    mAnnotationTypes(annotationTypes)
{
    mLayout = new QVBoxLayout(this);
    setLayout(mLayout);
    setupLayout();
}

void AnnotationMarkWidget::setupLayout()
{
    qDeleteAll(mMarks);
    mMarks.clear();

    for(int i=0; i<mAnnotationTypes->count(); i++)
    {
        Mark * mark = new Mark( mAnnotationTypes->at(i).mark(), this );

        TextBit annotation = mGlossItem->getAnnotation( mAnnotationTypes->at(i).label() ).annotation();
        if( annotation.text().isEmpty() )
        {
            mark->setToolTip( mAnnotationTypes->at(i).label() );
            mark->setStyleSheet("QLabel { color: lightgray; }");
        }
        else
        {
            mark->setToolTip( annotation.text() );
        }

        connect( mark, SIGNAL(doubleClicked(Mark*)), this, SLOT(markActivated(Mark*)) );

        mMarks << mark;
        mLayout->addWidget(mark);
    }
}

void AnnotationMarkWidget::markActivated( Mark * mark )
{
    emit annotationActivated( mAnnotationTypes->at( mMarks.indexOf(mark) ).label() );
}
