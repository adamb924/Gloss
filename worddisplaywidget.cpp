#include "worddisplaywidget.h"
#include "textbit.h"
#include "project.h"
#include "lingedit.h"
#include "glossline.h"
#include "databaseadapter.h"

#include <QtGui>
#include <QtDebug>

WordDisplayWidget::WordDisplayWidget( GlossItem *item, Qt::Alignment alignment, Project *project)
{
    mGlossItem = item;
    mAlignment = alignment;
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

    mLayout->addWidget(new QLabel(mGlossItem->baselineText()->text()));
    for(int i=0; i<mGlossLines.count(); i++)
    {
        LingEdit *edit = new LingEdit( TextBit( QString("") , mGlossLines.at(i).writingSystem(), mGlossItem->id() ), this);
        edit->setAlignment(calculateAlignment());
        mLayout->addWidget(edit);
        mEdits << edit;

        switch( mGlossLines.at(i).type() )
        {
        case GlossLine::Text:
            connect(edit, SIGNAL(stringChanged(TextBit)), mProject->dbAdapter(), SLOT(updateInterpretationTextForm(TextBit)));
            connect(edit, SIGNAL(stringChanged(TextBit)), this, SIGNAL(textChanged(TextBit)));
            break;
        case GlossLine::Gloss:
            connect(edit,SIGNAL(stringChanged(TextBit)), mProject->dbAdapter(), SLOT(updateInterpretationGloss(TextBit)));
            connect(edit, SIGNAL(stringChanged(TextBit)), this, SIGNAL(glossChanged(TextBit)));
            break;
        }
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
}

QSize WordDisplayWidget::sizeHint() const
{
    return QSize(128,128);
}

void WordDisplayWidget::fillData()
{
    if( mGlossItem->id() != -1 )
    {
        for(int i=0; i<mGlossLines.count();i++)
        {
            switch( mGlossLines.at(i).type() )
            {
            case GlossLine::Text:
                mEdits[i]->setText( mProject->dbAdapter()->getInterpretationTextForm(mGlossItem->id(), mGlossLines.at(i).writingSystem() ) );
                break;
            case GlossLine::Gloss:
                mEdits[i]->setText( mProject->dbAdapter()->getInterpretationGloss(mGlossItem->id(), mGlossLines.at(i).writingSystem() ) );
                break;
            }
        }
    }
}

GlossItem* WordDisplayWidget::glossItem() const
{
    return mGlossItem;
}

void WordDisplayWidget::updateEdit( const TextBit & bit, GlossLine::LineType type )
{
    LingEdit *line = getAppropriateEdit(bit, type);
    if( line != 0 )
        line->setText(bit.text());
}

LingEdit* WordDisplayWidget::getAppropriateEdit(const TextBit & bit, GlossLine::LineType type )
{
    for(int i=0; i<mGlossLines.count(); i++)
        if( mGlossLines.at(i).type() == type && mGlossLines.at(i).writingSystem()->flexString() == bit.writingSystem()->flexString() )
            return mEdits[i];
    return 0;
}

// TODO this has no effect
Qt::Alignment WordDisplayWidget::calculateAlignment() const
{
    if( mGlossItem->baselineText()->writingSystem()->layoutDirection() == Qt::LeftToRight )
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
