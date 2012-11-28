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

    // does this belong here or would that defeat the purpose?
    mConcordance.clear();
}

void InterlinearDisplayWidget::setLayoutFromText()
{
    clearData();

    for(int i=0; i< mText->baselineBits()->count(); i++)
    {
        FlowLayout *flowLayout = addLine();
        for(int j=0; j<mText->baselineBits()->at(i)->count(); j++)
        {
            WordDisplayWidget *wdw = addWordDisplayWidget(mText->baselineBits()->at(i)->at(j));
            flowLayout->addWidget(wdw);
        }
    }
    mLayout->addStretch(100);
}

WordDisplayWidget* InterlinearDisplayWidget::addWordDisplayWidget(TextBit *bit)
{
    // once this object is constructed, it will have an id
    WordDisplayWidget *wdw = new WordDisplayWidget( bit , mProject );
    // TODO this will not work
    connect(wdw,SIGNAL(idChanged(WordDisplayWidget*,qlonglong,qlonglong)),this,SLOT(updateConcordance(WordDisplayWidget*,qlonglong,qlonglong)));
    // this line is necessary because the signal from the constructor is emitted before the connection is made
    mConcordance.insert( wdw->textBit()->id() , wdw );

    // update concordant words
    connect(wdw,SIGNAL(glossChanged(TextBit)), this, SLOT(updateGloss(TextBit)));
    connect(wdw,SIGNAL(textChanged(TextBit)), this, SLOT(updateText(TextBit)));

    mWordDisplayWidgets << wdw;

    return wdw;
}

FlowLayout* InterlinearDisplayWidget::addLine()
{
    FlowLayout *flowLayout = new FlowLayout;
    mLineLayouts << flowLayout;
    mLayout->addLayout(flowLayout);
    return flowLayout;
}

void InterlinearDisplayWidget::updateConcordance( WordDisplayWidget *w, qlonglong oldId, qlonglong newId )
{
    mConcordance.remove(oldId,w);
    mConcordance.insert(newId,w);
}

void InterlinearDisplayWidget::updateGloss( const TextBit & bit )
{
    WordDisplayWidget* wdw;
    QList<WordDisplayWidget*> wdwList = mConcordance.values(bit.id());
    foreach(wdw, wdwList)
        wdw->updateEdit(bit,GlossLine::Gloss);
}

void InterlinearDisplayWidget::updateText( const TextBit & bit )
{
    WordDisplayWidget* wdw;
    QList<WordDisplayWidget*> wdwList = mConcordance.values(bit.id());
    foreach(wdw, wdwList)
        wdw->updateEdit(bit,GlossLine::Text);
}

void InterlinearDisplayWidget::updateMorphologicalAnalysis( const TextBit & bit , const QString & splitString )
{

}
