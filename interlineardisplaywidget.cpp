#include "interlineardisplaywidget.h"

#include <QtDebug>
#include <QtGui>
#include <QStringList>
#include <QLayout>

#include "flowlayout.h"
#include "textbit.h"
#include "worddisplaywidget.h"
#include "text.h"
#include "project.h"
#include "databaseadapter.h"
#include "phrase.h"
#include "interlinearlinelabel.h"

InterlinearDisplayWidget::InterlinearDisplayWidget(Text *text, Project *project, QWidget *parent) :
        QScrollArea(parent)
{
    mText = text;
    mProject = project;

    mCurrentLine = -1;

    mLayout = new QVBoxLayout;
    QWidget *theWidget = new QWidget(this);
    theWidget->setLayout(mLayout);

    setWidgetResizable(true);
    setBackgroundRole(QPalette::Light);
    this->setWidget( theWidget );
}

InterlinearDisplayWidget::~InterlinearDisplayWidget()
{
}

void InterlinearDisplayWidget::clearData()
{
    qDeleteAll(mLineLayouts);
    mLineLayouts.clear();
}

void InterlinearDisplayWidget::addLineLabel( int i , QLayout * flowLayout  )
{
    InterlinearLineLabel *lineNumber = new InterlinearLineLabel(i, QString("%1").arg(i+1), this);
    connect(lineNumber, SIGNAL(approveAll(int)), this, SLOT(approveAll(int)));
    connect(lineNumber, SIGNAL(playSound(int)), this, SLOT(playSound(int)));
    flowLayout->addWidget(lineNumber);
}

QLayout* InterlinearDisplayWidget::addLine()
{
    FlowLayout *flowLayout = new FlowLayout( mInterlinearDisplayLines.first().writingSystem().layoutDirection() , 0, 5 , 5 , 5 );
    mLineLayouts << flowLayout;
    mLayout->addLayout(flowLayout);
    return flowLayout;
}

void InterlinearDisplayWidget::saveText()
{
    mText->saveText();
}

void InterlinearDisplayWidget::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);
    menu.addAction(tr("Save this text"), this, SLOT(saveText()), QKeySequence("Ctrl+Shift+S"));
    menu.exec(event->globalPos());
}

void InterlinearDisplayWidget::scrollToLine(int line)
{
    if( line < mLineLayouts.count() )
    {
        bool after = line > mCurrentLine;
        mCurrentLine = line;
        if( mLineLayouts.at(mCurrentLine)->count() > 1 )
        {
            if( after )
            {
                ensureWidgetVisible( mLineLayouts.at(mCurrentLine)->itemAt( mLineLayouts.at(mCurrentLine)->count() - 1 )->widget() , 0 , 300 );
            }
            else
            {
                ensureWidgetVisible( mLineLayouts.at(mCurrentLine)->itemAt(0)->widget() , 0 , 300 );
            }
        }
    }
}

void InterlinearDisplayWidget::scrollContentsBy ( int dx, int dy )
{
    QScrollArea::scrollContentsBy(dx, dy);
    if( dy != 0 )
    {

    }
}

void InterlinearDisplayWidget::addPhrasalGlossLines( int i )
{
    for(int j=0; j<mPhrasalGlossLines.count(); j++)
    {
        TextBit bit = mText->phrases()->at(i)->gloss( mPhrasalGlossLines.at(j).writingSystem() );
        LingEdit *edit = addPhrasalGlossLine( bit );
        edit->matchTextAlignmentTo( mText->baselineWritingSystem().layoutDirection() );
        connect( edit, SIGNAL(stringChanged(TextBit)), mText->phrases()->at(i), SLOT(setPhrasalGloss(TextBit)) );
    }
}

LingEdit* InterlinearDisplayWidget::addPhrasalGlossLine( const TextBit & gloss )
{
    LingEdit *edit = new LingEdit( gloss , this);
    mLayout->addWidget(edit);
    mPhrasalGlossEdits << edit;
    return edit;
}

void InterlinearDisplayWidget::approveAll(int lineNumber)
{
    if( lineNumber >= mText->phrases()->count() )
        return;
    for(int i=0; i < mText->phrases()->at(lineNumber)->count(); i++)
        mText->phrases()->at(lineNumber)->at(i)->setApprovalStatus(GlossItem::Approved);
}

void InterlinearDisplayWidget::playSound(int lineNumber)
{
    // TODO implement this
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
