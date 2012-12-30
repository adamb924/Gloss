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
    QLabel *lineNumber = new QLabel(QString("%1").arg(i+1), this);
    lineNumber->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lineNumber->setMinimumSize(30, 30);
    flowLayout->addWidget(lineNumber);
}

QLayout* InterlinearDisplayWidget::addLine()
{
    FlowLayout *flowLayout = new FlowLayout( mText->baselineWritingSystem().layoutDirection() , 0, 5 , 5 , 5 );
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
