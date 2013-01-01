#include "analysisdisplaywidget.h"

#include <QtGui>
#include <QtDebug>

#include "project.h"
#include "text.h"
#include "worddisplaywidget.h"
#include "databaseadapter.h"

AnalysisDisplayWidget::AnalysisDisplayWidget(Text *text, Project *project, QWidget *parent) : InterlinearDisplayWidget(text, project, parent)
{
    mPhrasalGlossLines = mProject->dbAdapter()->analysisPhrasalGlossLines();
    mInterlinearDisplayLines = mProject->dbAdapter()->analysisInterlinearLines();

    // TODO connect to some signal when the analysis baseline text forms change

    setLayoutFromText();
}

void AnalysisDisplayWidget::setLayoutFromText()
{
    QProgressDialog progress(tr("Creating interface for %1...").arg(mText->name()), "Cancel", 0, mText->phrases()->count(), 0);
    progress.setWindowModality(Qt::WindowModal);

    for(int i=0; i< mText->phrases()->count(); i++)
    {
        progress.setValue(i);

        QLayout *flowLayout;

        if( i >= mLineLayouts.count() ) // there is no layout here
        {
            flowLayout = addLine();
            addPhrasalGlossLines(i);
        }
        else if( mText->phrases()->at(i)->guiRefreshRequest() )
        {
            flowLayout = mLineLayouts.at(i);
            clearWidgets( flowLayout );
        }
        else
        {
            continue;
        }

        addLineLabel(i, flowLayout);
        if( flowLayout->count() == 1 ) // it's either new or has been cleared for a refresh
            addWordWidgets(i, flowLayout);

        mText->phrases()->at(i)->setGuiRefreshRequest(false);
    }
    progress.setValue(mText->phrases()->count());
}

void AnalysisDisplayWidget::clearWidgets(QLayout * layout)
{
    if( layout->count() == 0 )
        return;
    QLayoutItem * item;
    while( ( item = layout->takeAt(0) ) != 0 )
    {
        mWordDisplayWidgets.remove(item->widget());
        delete item->widget();
        delete item;
    }
}

void AnalysisDisplayWidget::addWordWidgets( int i , QLayout * flowLayout )
{
    for(int j=0; j<mText->phrases()->at(i)->count(); j++)
    {
        WordDisplayWidget *wdw = addWordDisplayWidget(mText->phrases()->at(i)->at(j));
//        connect( mText->phrases()->at(i)->at(j), SIGNAL(interpretationIdChanged(qlonglong)), wdw, SLOT(sendConcordanceUpdates()) );
//        connect( wdw, SIGNAL(alternateInterpretationAvailableFor(int)), this, SLOT(otherInterpretationsAvailableFor(int)) );
        flowLayout->addWidget(wdw);
    }
}

WordDisplayWidget* AnalysisDisplayWidget::addWordDisplayWidget(GlossItem *item)
{
    // TODO don't hardwire the text direction like this
    WordDisplayWidget *wdw = new WordDisplayWidget( item , Qt::AlignLeft, mInterlinearDisplayLines , this, mProject->dbAdapter() );
    mWordDisplayWidgets << wdw;
//    mWdwByInterpretationId.insert( item->id() , wdw );
//    mTextFormConcordance.unite( wdw->textFormEdits() );
//    mGlossConcordance.unite( wdw->glossEdits() );

//    connect( wdw, SIGNAL(glossIdChanged(LingEdit*,qlonglong)), this, SLOT(updateGlossFormConcordance(LingEdit*,qlonglong)));
//    connect( wdw, SIGNAL(textFormIdChanged(LingEdit*,qlonglong)), this, SLOT(updateTextFormConcordance(LingEdit*,qlonglong)));

    return wdw;
}
