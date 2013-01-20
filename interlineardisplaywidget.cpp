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
#include "generictextinputdialog.h"

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
    InterlinearLineLabel *lineNumber = new InterlinearLineLabel(i, QString("%1").arg(i+1), mText->phrases()->at(i)->annotation()->isValid(), this);
    connect(lineNumber, SIGNAL(approveAll(int)), this, SLOT(approveAll(int)));
    connect(lineNumber, SIGNAL(playSound(int)), this, SLOT(playSound(int)));
    connect(lineNumber, SIGNAL(editLine(int)), this, SLOT(editLine(int)));

    flowLayout->addWidget(lineNumber);
    mLineLabels.insert(i, lineNumber);
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
        connect( edit, SIGNAL(stringChanged(TextBit,LingEdit*)), mText->phrases()->at(i), SLOT(setPhrasalGloss(TextBit)) );
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
    for(int i=0; i < mText->phrases()->at(lineNumber)->glossItemCount(); i++)
        mText->phrases()->at(lineNumber)->glossItemAt(i)->setApprovalStatus(GlossItem::Approved);
}

void InterlinearDisplayWidget::playSound(int lineNumber)
{
    mText->playSoundForLine(lineNumber);
}

void InterlinearDisplayWidget::editLine(int lineNumber)
{
    // Launch a dialog requesting input
    GenericTextInputDialog dialog( TextBit( mText->baselineTextForLine(lineNumber) , mText->baselineWritingSystem() ) , this);
    dialog.setWindowTitle(tr("Edit baseline text - Line %1").arg(lineNumber+1));
    if( dialog.exec() == QDialog::Accepted )
    {
        mText->setBaselineTextForLine(lineNumber, dialog.text() );
        setLayoutAsAppropriate();
    }
}

void InterlinearDisplayWidget::setLayoutAsAppropriate()
{
    if( mLines.isEmpty() )
        setLayoutFromText();
    else
        setLayoutFromText(mLines);
    setEnabled(true);
}

void InterlinearDisplayWidget::clearWidgetsFromLine(int lineNumber)
{
    if( lineNumber >= mLineLayouts.count() )
    {
        qWarning() << "Layout index out of bounds:" << lineNumber << mLineLayouts.count();
        return;
    }
    QLayout *layout = mLineLayouts.at(lineNumber);

    if( lineNumber >= mLineLabels.count() )
    {
        qWarning() << "Line label index out of bounds:" << lineNumber << mLineLabels.count();
        return;
    }
    QWidget *lineLabel = mLineLabels.takeAt(lineNumber);
    delete lineLabel;
    layout->removeWidget(lineLabel);

    QListIterator<QWidget*> iter =  QListIterator<QWidget*>( mWordDisplayWidgets.values(lineNumber) );
    while(iter.hasNext())
    {
        QWidget *wdw = iter.next();
        layout->removeWidget(wdw);
        wdw->deleteLater();
        mWordDisplayWidgets.remove( lineNumber, wdw );
    }
}
