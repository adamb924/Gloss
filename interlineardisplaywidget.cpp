#include "interlineardisplaywidget.h"

#include <QtDebug>
#include <QtGui>
#include <QStringList>

#include "flowlayout.h"
#include "textbit.h"
#include "worddisplaywidget.h"
#include "text.h"
#include "project.h"

InterlinearDisplayWidget::InterlinearDisplayWidget(Text *text, Project *project, QWidget *parent) :
    QWidget(parent)
{
    mText = text;
    mProject = project;
    mLayout = new QVBoxLayout(this);
    setLayout(mLayout);

    connect( text, SIGNAL(baselineTextChanged(QString)), this, SLOT(baselineTextUpdated(QString)));

    if( mText->glossItems()->length() > 0 )
        setLayoutFromText();
}

InterlinearDisplayWidget::~InterlinearDisplayWidget()
{
    clearData();
}

void InterlinearDisplayWidget::baselineTextUpdated(const QString & baselineText)
{
    // TODO it's not clear to me that this is the best way to do this
    setLayoutFromText();
}

void InterlinearDisplayWidget::clearData()
{
    qDeleteAll(mWordDisplayWidgets);
    mWordDisplayWidgets.clear();
    qDeleteAll(mLineLayouts);
    mLineLayouts.clear();

    mGlossConcordance.clear();
    mTextFormConcordance.clear();
}

void InterlinearDisplayWidget::setLayoutFromText()
{
    clearData();

    for(int i=0; i< mText->glossItems()->count(); i++)
    {
        QLayout *flowLayout = addLine();
        for(int j=0; j<mText->glossItems()->at(i)->count(); j++)
        {
            WordDisplayWidget *wdw = addWordDisplayWidget(mText->glossItems()->at(i)->at(j));
            flowLayout->addWidget(wdw);
        }
    }
    mLayout->addStretch(100);
}

WordDisplayWidget* InterlinearDisplayWidget::addWordDisplayWidget(GlossItem *item)
{
    WordDisplayWidget *wdw = new WordDisplayWidget( item , mText->writingSystem().layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight , this, mProject->dbAdapter() );
    mWordDisplayWidgets << wdw;
    mTextFormConcordance.unite( wdw->textFormEdits() );
    mGlossConcordance.unite( wdw->glossEdits() );

    connect( wdw, SIGNAL(glossIdChanged(LingEdit*,qlonglong)), this, SLOT(updateGlossFormConcordance(LingEdit*,qlonglong)));
    connect( wdw, SIGNAL(textFormIdChanged(LingEdit*,qlonglong)), this, SLOT(updateTextFormConcordance(LingEdit*,qlonglong)));

    return wdw;
}

QLayout* InterlinearDisplayWidget::addLine()
{
    FlowLayout *flowLayout = new FlowLayout;
    mLineLayouts << flowLayout;
    mLayout->addLayout(flowLayout);
    return flowLayout;
}

void InterlinearDisplayWidget::updateGloss( const TextBit & bit )
{
    LingEdit* edit;
    QList<LingEdit*> editList = mGlossConcordance.values(bit.id());
    foreach(edit, editList)
        edit->setText( bit.text() );
}

void InterlinearDisplayWidget::updateText( const TextBit & bit )
{
    LingEdit* edit;
    QList<LingEdit*> editList = mTextFormConcordance.values(bit.id());
    foreach(edit, editList)
        edit->setText( bit.text() );
}

void InterlinearDisplayWidget::updateMorphologicalAnalysis( const TextBit & bit , const QString & splitString )
{

}

void InterlinearDisplayWidget::updateTextFormConcordance(LingEdit * edit, qlonglong newId)
{
    qlonglong oldId = mTextFormConcordance.key( edit );
    mTextFormConcordance.remove(oldId, edit);
    mTextFormConcordance.insert(newId, edit);
}

void InterlinearDisplayWidget::updateGlossFormConcordance(LingEdit * edit, qlonglong newId)
{
    qlonglong oldId = mGlossConcordance.key( edit );
    mGlossConcordance.remove(oldId, edit);
    mGlossConcordance.insert(newId, edit);
}
