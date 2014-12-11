#include "texttabwidget.h"

#include "databaseadapter.h"
#include "text.h"
#include "interlineardisplaywidget.h"
#include "syntacticparsingwidget.h"

#include <QCloseEvent>
#include <QLabel>
#include <QMessageBox>

TextTabWidget::TextTabWidget(Text *text, Project *project, View::Type type, const QList<int> & lines, const QList<Focus> & foci, QWidget *parent) :
    QTabWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    mText = text;
    mProject = project;
    const View *view = mProject->view(type);

    if( view == 0 )
    {
        QMessageBox::critical(this, tr("Error"), tr("There is no valid view available. Something is likely wrong with your configuration.xml file."));
    }

    for(int i=0; i<view->tabs()->count(); i++)
    {
        if( view->tabs()->at(i)->type() == Tab::InterlinearDisplay )
        {
            InterlinearDisplayWidget * idw = new InterlinearDisplayWidget( view->tabs()->at(i), mText, mProject, this);
            idw->setFocus(foci);
            idw->setLines(lines);

            connect( text, SIGNAL(glossItemsChanged()), idw, SLOT(setLayoutFromText()) );
            connect( text, SIGNAL(phraseRefreshNeeded(int)), idw, SLOT(requestLineRefresh(int)) );
            connect( text, SIGNAL(phrasalGlossChanged(int,TextBit)), this, SLOT(updatePhrasalGloss(int,TextBit)) );

            connect( text, SIGNAL(guiRefreshRequest()), idw, SLOT(resetGui()) );

            mIdwTabs << idw;
            addTab( idw, view->tabs()->at(i)->name() );
        }
        else if ( view->tabs()->at(i)->type() == Tab::SyntacticParsing )
        {
            SyntacticParsingWidget * spw = new SyntacticParsingWidget(mText, view->tabs()->at(i), mProject, this);
            addTab( spw , view->tabs()->at(i)->name() );
        }
    }

    if( count() == 1 )
        tabBar()->hide();

    setWindowTitle(tr("%1 [%2]").arg(mText->name()).arg(view->name()));
}

TextTabWidget::~TextTabWidget()
{
    if( mProject->memoryMode() == Project::OneAtATime )
        mProject->closeText(mText);
}

void TextTabWidget::updatePhrasalGloss(int lineNumber, const TextBit &bit)
{
    for(int i=0; i<mIdwTabs.count(); i++)
        mIdwTabs.at(i)->setPhrasalGloss(lineNumber, bit);
}

void TextTabWidget::closeEvent(QCloseEvent *event)
{
    saveText();
    event->accept();
}

void TextTabWidget::setLines(const QList<int> & lines)
{
    for(int i=0; i<mIdwTabs.count(); i++)
    {
        mIdwTabs.at(i)->setLines(lines);
        mIdwTabs.at(i)->verticalScrollBar()->triggerAction(QScrollBar::SliderToMinimum);
    }
}

void TextTabWidget::saveText()
{
    mText->saveText(false,true,false);
}

Text * TextTabWidget::text()
{
    return mText;
}

void TextTabWidget::setFocus( const QList<Focus> & foci )
{
    for(int i=0; i<mIdwTabs.count(); i++)
        mIdwTabs.at(i)->setFocus(foci);
}
