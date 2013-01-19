#include "glossdisplaywidget.h"

#include <QtGui>
#include <QList>

#include "project.h"
#include "text.h"
#include "lingedit.h"
#include "worddisplaywidget.h"
#include "databaseadapter.h"

GlossDisplayWidget::GlossDisplayWidget(Text *text, Project *project, QWidget *parent) : InterlinearDisplayWidget(text, project, parent)
{
    mPhrasalGlossLines = mProject->dbAdapter()->glossPhrasalGlossLines();
    mInterlinearDisplayLines = mProject->dbAdapter()->glossInterlinearLines();

    mLines.clear();

    setLayoutAsAppropriate();
}

GlossDisplayWidget::GlossDisplayWidget(Text *text, Project *project, QList<int> lines, QWidget *parent) : InterlinearDisplayWidget(text, project, parent)
{
    mPhrasalGlossLines = mProject->dbAdapter()->glossPhrasalGlossLines();
    mInterlinearDisplayLines = mProject->dbAdapter()->glossInterlinearLines();

    mLines = lines;

    setLayoutAsAppropriate();
}

GlossDisplayWidget::~GlossDisplayWidget()
{

}

void GlossDisplayWidget::baselineTextUpdated(const QString & baselineText)
{
    setLayoutFromText();
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

void GlossDisplayWidget::setLayoutFromText(QList<int> lines)
{
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
            clearWidgets( flowLayout );
        }

        addLineLabel(i, flowLayout);
        if( flowLayout->count() == 1 ) // it's either new or has been cleared for a refresh
            addWordWidgets(i, flowLayout);

        mText->phrases()->at(i)->setGuiRefreshRequest(false);
    }
}

void GlossDisplayWidget::addWordWidgets( int i , QLayout * flowLayout )
{
    for(int j=0; j<mText->phrases()->at(i)->glossItemCount(); j++)
    {
        WordDisplayWidget *wdw = addWordDisplayWidget(mText->phrases()->at(i)->glossItemAt(j), mText->phrases()->at(i));
        flowLayout->addWidget(wdw);
    }
}

void GlossDisplayWidget::clearData()
{
    InterlinearDisplayWidget::clearData();

    qDeleteAll(mWordDisplayWidgets);
    mWordDisplayWidgets.clear();

    qDeleteAll(mPhrasalGlossEdits);
    mPhrasalGlossEdits.clear();
}

WordDisplayWidget* GlossDisplayWidget::addWordDisplayWidget(GlossItem *item, Phrase *phrase)
{
    WordDisplayWidget *wdw = new WordDisplayWidget( item , mText->baselineWritingSystem().layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight, mInterlinearDisplayLines, mProject->dbAdapter() );
    mWordDisplayWidgets << wdw;

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
            mWordDisplayWidgets.remove(wdw);
            // I'm not sure why it crashes when I delete the InterlinearLineLabel, but since those objects are parented to the widget anyway, they should eventually be deleted just the same.
            delete item->widget();
        }
        delete item;
    }
}
