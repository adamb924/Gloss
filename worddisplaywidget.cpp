#include "worddisplaywidget.h"
#include "textbit.h"
#include "project.h"
#include "lingedit.h"
#include "glossline.h"
#include "databaseadapter.h"
#include "interlineardisplaywidget.h"

#include <QtGui>
#include <QtDebug>

WordDisplayWidget::WordDisplayWidget( GlossItem *item, Qt::Alignment alignment, InterlinearDisplayWidget *ildw, Project *project)
{
    mGlossItem = item;
    mAlignment = alignment;
    mInterlinearDisplayWidget = ildw;
    mProject = project;

    mGlossLines = mProject->glossLines();

    setupLayout();

    setMinimumSize(128,128);

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));

    fillData();
}

void WordDisplayWidget::setupLayout()
{
    mLayout = new QVBoxLayout;
    setLayout(mLayout);

    mEdits.clear();

    QLabel *baselineWord = new QLabel(mGlossItem->baselineText().text());
    baselineWord->setStyleSheet(QString("font-family: %1; font-size: %2pt;").arg(mGlossItem->baselineText().writingSystem().fontFamily()).arg(mGlossItem->baselineText().writingSystem().fontSize()));

    mLayout->addWidget(baselineWord);
    for(int i=0; i<mGlossLines.count(); i++)
    {
        // TODO rethink how to do this
//        if( mGlossItem->baselineText()->writingSystem()->flexString() != mGlossLines.at(i).writingSystem()->flexString() )
//        {
            LingEdit *edit = new LingEdit( TextBit( QString("") , mGlossLines.at(i).writingSystem(), mGlossItem->id() ), this);
            edit->setAlignment(calculateAlignment());
            mLayout->addWidget(edit);
            mEdits.insert(mGlossLines.at(i).writingSystem(), edit);

            switch( mGlossLines.at(i).type() )
            {
            case GlossLine::Text:
                connect(edit, SIGNAL(stringChanged(TextBit)), mProject->dbAdapter(), SLOT(updateInterpretationTextForm(TextBit)));
                // when the edit's text is changed, a signal is emitted, which is picked up by InterlinearDisplayWidget
                connect(edit, SIGNAL(stringChanged(TextBit)), mInterlinearDisplayWidget, SLOT(updateText(TextBit)));
                break;
            case GlossLine::Gloss:
                connect(edit,SIGNAL(stringChanged(TextBit)), mProject->dbAdapter(), SLOT(updateInterpretationGloss(TextBit)));
                connect(edit, SIGNAL(stringChanged(TextBit)), mInterlinearDisplayWidget, SLOT(updateGloss(TextBit)));
                break;
            }
//        }
    }
}


void WordDisplayWidget::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);
    menu.addAction(tr("New gloss..."),this,SLOT(newGloss()));
    menu.exec(event->globalPos());
}

void WordDisplayWidget::newGloss()
{
    qlonglong id = mProject->dbAdapter()->newInterpretation( mGlossItem->baselineText() );
    mGlossItem->setInterpretation(id);
    fillData();
}

void WordDisplayWidget::fillData()
{
    if( mGlossItem->id() != -1 )
    {
        for(int i=0; i<mGlossLines.count();i++)
        {
            QString form;
            switch( mGlossLines.at(i).type() )
            {
            case GlossLine::Text:
                form = mGlossItem->textItems()->value( mGlossLines.at(i).writingSystem() );
                break;
            case GlossLine::Gloss:
                form = mGlossItem->glossItems()->value( mGlossLines.at(i).writingSystem() );
                break;
            }
            mEdits[mGlossLines.at(i).writingSystem()]->setText( form );
            mEdits[mGlossLines.at(i).writingSystem()]->setText( form );
        }
    }
}

void WordDisplayWidget::updateEdit( const TextBit & bit, GlossLine::LineType type )
{
    LingEdit *line = mEdits.value(bit.writingSystem());
    if( line != 0 )
        line->setText(bit.text());
}

// TODO this has no effect
Qt::Alignment WordDisplayWidget::calculateAlignment() const
{
    if( mGlossItem->baselineText().writingSystem().layoutDirection() == Qt::LeftToRight )
    {
        if( mAlignment == Qt::AlignLeft )
        {
            return Qt::AlignLeft;
        }
        else
        {
            return Qt::AlignRight;
        }
    }
    else // Qt::RightToLeft
    {
        if( mAlignment == Qt::AlignLeft )
        {
            return Qt::AlignLeft;
        }
        else
        {
            return Qt::AlignLeft;
        }
    }
}
