#include "textdisplaywidget.h"

#include "databaseadapter.h"
#include "text.h"
#include "interlineardisplaywidget.h"

#include <QCloseEvent>
#include <QLabel>
#include <QMessageBox>

TextDisplayWidget::TextDisplayWidget(Text *text, Project *project, View::Type type, const QList<int> & lines, const QList<Focus> & foci, QWidget *parent) :
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

    if( count() == 1 )
        tabBar()->hide();

    setWindowTitle(mText->name());
}

TextDisplayWidget::~TextDisplayWidget()
{
    if( mProject->memoryMode() == Project::OneAtATime )
        mProject->closeText(mText);
}

void TextDisplayWidget::updatePhrasalGloss(int lineNumber, const TextBit &bit)
{
    for(int i=0; i<mIdwTabs.count(); i++)
        mIdwTabs.at(i)->setPhrasalGloss(lineNumber, bit);
}

void TextDisplayWidget::closeEvent(QCloseEvent *event)
{
    saveText();
    event->accept();
}

void TextDisplayWidget::setLines(const QList<int> & lines)
{
    for(int i=0; i<mIdwTabs.count(); i++)
        mIdwTabs.at(i)->setLines(lines);
}

void TextDisplayWidget::saveText()
{
    mText->saveText(false,false,true,false);
}

Text * TextDisplayWidget::text()
{
    return mText;
}

void TextDisplayWidget::setFocus( const QList<Focus> & foci )
{
    for(int i=0; i<mIdwTabs.count(); i++)
        mIdwTabs.at(i)->setFocus(foci);
}
