#include "glossdisplaywidget.h"

#include <QtGui>
#include <QList>

#include "project.h"
#include "text.h"
#include "lingedit.h"
#include "worddisplaywidget.h"
#include "databaseadapter.h"

GlossDisplayWidget::GlossDisplayWidget(const QList<InterlinearItemType> & interlinearDisplayLines, const QList<InterlinearItemType> & phrasalGlossLines, Text *text, Project *project, QWidget *parent) : InterlinearDisplayWidget(interlinearDisplayLines, phrasalGlossLines, text, project, parent)
{
    mLines.clear();

    setLayoutAsAppropriate();
}

GlossDisplayWidget::GlossDisplayWidget(const QList<InterlinearItemType> & interlinearDisplayLines, const QList<InterlinearItemType> & phrasalGlossLines, Text *text, Project *project, QList<int> lines, QWidget *parent) : InterlinearDisplayWidget(interlinearDisplayLines, phrasalGlossLines, text, project, parent)
{
    mLines = lines;

    setLayoutAsAppropriate();
}

GlossDisplayWidget::~GlossDisplayWidget()
{

}

void GlossDisplayWidget::setLayoutFromText()
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
            clearWidgetsFromLine(i);
        }
        else
        {
            continue;
        }

        if( flowLayout->isEmpty() ) // it's either new or has been cleared for a refresh
        {
            addLineLabel(i, flowLayout);
            addWordWidgets(i, flowLayout);
        }

        mText->phrases()->at(i)->setGuiRefreshRequest(false);
    }
    progress.setValue(mText->phrases()->count());
}

void GlossDisplayWidget::setLayoutFromText(QList<int> lines)
{
    // TODO merge this with setLayoutFromText, as it should have been from the beginning
    QListIterator<int> iter(lines);
    while(iter.hasNext())
    {
        int i = iter.next();

        QLayout *flowLayout;

        if( i >= mLineLayouts.count() ) // there is no layout here
        {
            flowLayout = addLine();
            addPhrasalGlossLines(i);
        }
        else
        {
            flowLayout = mLineLayouts.at(i);
            clearWidgetsFromLine(i);
        }

        if( flowLayout->isEmpty() )
        {
            addLineLabel(i, flowLayout);
            addWordWidgets(i, flowLayout);
        }

        mText->phrases()->at(i)->setGuiRefreshRequest(false);
    }
}

void GlossDisplayWidget::addWordWidgets( int i , QLayout * flowLayout )
{
    for(int j=0; j<mText->phrases()->at(i)->glossItemCount(); j++)
    {
        WordDisplayWidget *wdw = addWordDisplayWidget(mText->phrases()->at(i)->glossItemAt(j), mText->phrases()->at(i));
        mWordDisplayWidgets.insert(i, wdw);
        flowLayout->addWidget(wdw);
    }
}

WordDisplayWidget* GlossDisplayWidget::addWordDisplayWidget(GlossItem *item, Phrase *phrase)
{
    WordDisplayWidget *wdw = new WordDisplayWidget( item , mText->baselineWritingSystem().layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight, mInterlinearDisplayLines, mProject->dbAdapter(), this );

    connect( wdw, SIGNAL(splitWidgetInTwo(GlossItem*,TextBit,TextBit)), phrase, SLOT(splitGlossInTwo(GlossItem*,TextBit,TextBit)) );
    connect( wdw, SIGNAL(mergeGlossItemWithNext(GlossItem*)), phrase, SLOT(mergeGlossItemWithNext(GlossItem*)));
    connect( wdw, SIGNAL(mergeGlossItemWithPrevious(GlossItem*)), phrase, SLOT(mergeGlossItemWithPrevious(GlossItem*)));

    return wdw;
}

void GlossDisplayWidget::clearWidgets(QLayout * layout)
{
    while( layout->count() > 0 )
    {
        QLayoutItem * item = layout->takeAt(0);
        WordDisplayWidget *wdw = qobject_cast<WordDisplayWidget*>(item->widget());
        if( wdw != 0 )
        {
//            mWordDisplayWidgets.remove(wdw);
            // I'm not sure why it crashes when I delete the InterlinearLineLabel, but since those objects are parented to the widget anyway, they should eventually be deleted just the same.
            item->widget()->deleteLater();;
        }
        delete item;
    }
}
