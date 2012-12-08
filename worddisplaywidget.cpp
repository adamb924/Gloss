#include "worddisplaywidget.h"
#include "textbit.h"
#include "project.h"
#include "lingedit.h"
#include "glossline.h"
#include "databaseadapter.h"
#include "interlineardisplaywidget.h"

#include <QtGui>
#include <QtDebug>
#include <QActionGroup>

WordDisplayWidget::WordDisplayWidget( GlossItem *item, Qt::Alignment alignment, InterlinearDisplayWidget *ildw, DatabaseAdapter *dbAdapter)
{
    mDbAdapter = dbAdapter;
    mGlossItem = item;
    mAlignment = alignment;
    mInterlinearDisplayWidget = ildw;

    mGlossLines = mDbAdapter->glossLines();

    setupLayout();

    setMinimumSize(128,128);

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));

    connect( mGlossItem, SIGNAL(approvalStatusChanged(ApprovalStatus)), this, SLOT(updateBaselineLabelStyle()) );
    connect( mGlossItem, SIGNAL(candidateStatusChanged(CandidateStatus)), this, SLOT(updateBaselineLabelStyle()) );
    connect( mGlossItem, SIGNAL(fieldsChanged()), this, SLOT(fillData()) );

    fillData();

    updateBaselineLabelStyle();
}

void WordDisplayWidget::setupLayout()
{
    mLayout = new QVBoxLayout;
    setLayout(mLayout);

    mEdits.clear();

    //    QLineEdit *edit = new QLineEdit;
    //    edit->setAlignment(Qt::AlignRight);
    //    mLayout->addWidget(edit);


    mBaselineWordLabel = new QLabel(mGlossItem->baselineText().text());
    mBaselineWordLabel->setAlignment( mGlossItem->writingSystem().layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight );
    updateBaselineLabelStyle();

    mLayout->addWidget(mBaselineWordLabel);
    for(int i=0; i<mGlossLines.count(); i++)
    {
        LingEdit *edit = new LingEdit( TextBit( QString("") , mGlossLines.at(i).writingSystem(), mGlossItem->id() ), this);
        edit->setAlignment(calculateAlignment( mGlossItem->writingSystem().layoutDirection() , mGlossLines.at(i).writingSystem().layoutDirection() ) );
        mLayout->addWidget(edit);
        mEdits.insert(mGlossLines.at(i).writingSystem(), edit);

        switch( mGlossLines.at(i).type() )
        {
        case GlossLine::Text:
            // when the edit's text is changed, a signal is emitted, which is picked up by InterlinearDisplayWidget
            connect(edit, SIGNAL(stringChanged(TextBit)), mInterlinearDisplayWidget, SLOT(updateText(TextBit)));
            connect(edit,SIGNAL(stringChanged(TextBit)), mGlossItem, SLOT(updateText(TextBit)) );
            break;
        case GlossLine::Gloss:
            connect(edit, SIGNAL(stringChanged(TextBit)), mInterlinearDisplayWidget, SLOT(updateGloss(TextBit)));
            connect(edit,SIGNAL(stringChanged(TextBit)), mGlossItem, SLOT(updateGloss(TextBit)) );
            break;
        }
        connect( mGlossItem , SIGNAL(idChanged(qlonglong)), edit, SLOT(changeId(qlonglong)) );
    }
}


void WordDisplayWidget::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);
    menu.addAction(tr("New interpretation..."),this,SLOT(newGloss()));

    // TODO verify that this mess is really necessary
    QAction *approved = new QAction(tr("Approved"),&menu);
    approved->setCheckable(true);
    approved->setChecked(mGlossItem->approvalStatus() == GlossItem::Approved );
    QActionGroup *gApproved = new QActionGroup(&menu);
    gApproved->addAction(approved);
    menu.addAction(approved);
    connect( gApproved, SIGNAL(triggered(QAction*)) , mGlossItem, SLOT(toggleApproval()) );

    QActionGroup *gCandidates = new QActionGroup(&menu);
    QHash<qlonglong,QString> candidates = mDbAdapter->candidateInterpretationWithSummaries( mGlossItem->baselineText() );

    if(candidates.count() > 1)
    {
        menu.addSeparator();
        QHashIterator<qlonglong,QString> iter(candidates);
        while(iter.hasNext())
        {
            iter.next();
            QAction *candidate = new QAction( iter.value(), &menu );
            candidate->setCheckable(true);
            if( mGlossItem->id() == iter.key() )
                candidate->setChecked(true);
            candidate->setData(iter.key());
            gCandidates->addAction(candidate);
            menu.addAction(candidate);
        }
        connect(gCandidates, SIGNAL(triggered(QAction*)), this, SLOT(selectDifferentCandidate(QAction*)));
    }

    menu.exec(event->globalPos());
}

void WordDisplayWidget::newGloss()
{
    qlonglong id = mDbAdapter->newInterpretation( mGlossItem->baselineText() );
    mGlossItem->setInterpretation(id);
    mGlossItem->setCandidateStatus(GlossItem::MultipleOption);
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
                form = mGlossItem->textForms()->value( mGlossLines.at(i).writingSystem() );
                break;
            case GlossLine::Gloss:
                form = mGlossItem->glosses()->value( mGlossLines.at(i).writingSystem() );
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
Qt::Alignment WordDisplayWidget::calculateAlignment( Qt::LayoutDirection match , Qt::LayoutDirection current ) const
{
    if( match == Qt::LeftToRight )
    {
        if( current == Qt::LeftToRight )
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
        if( current == Qt::LeftToRight )
        {
            return Qt::AlignRight;
        }
        else
        {
            return Qt::AlignLeft;
        }
    }
}

void WordDisplayWidget::updateBaselineLabelStyle()
{
    QString color;
    if ( mGlossItem->approvalStatus() == GlossItem::Unapproved && mGlossItem->candidateStatus() == GlossItem::SingleOption )
        color = "#9aff67";
    else if ( mGlossItem->approvalStatus() == GlossItem::Unapproved && mGlossItem->candidateStatus() == GlossItem::MultipleOption )
        color = "#fff6a8";
    else
        color = "#ffffff";
    mBaselineWordLabel->setStyleSheet(QString("font-family: %1; font-size: %2pt; background-color: %3;").arg(mGlossItem->baselineText().writingSystem().fontFamily()).arg(mGlossItem->baselineText().writingSystem().fontSize()).arg(color));
}

void WordDisplayWidget::mouseDoubleClickEvent ( QMouseEvent * event )
{
    mGlossItem->toggleApproval();
}

void WordDisplayWidget::selectDifferentCandidate(QAction *action)
{
    mGlossItem->setInterpretation( action->data().toLongLong() );
}
