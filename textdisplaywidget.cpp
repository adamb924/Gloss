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

    connect(this,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));

    for(int i=0; i<view->tabs()->count(); i++)
    {
        // TODO really these should just be stored as pointers. This will waste memory; I'm just doing it to have it done with for now.
        Tab * tab = new Tab( * view->tabs()->at(i) );
        InterlinearDisplayWidget * idw = new InterlinearDisplayWidget( tab, mText, mProject, this);
        idw->setFocus(foci);
        idw->setLines(lines);

        connect( text, SIGNAL(baselineTextChanged(QString)), idw, SLOT(setLayoutFromText()) );
        connect( text, SIGNAL(glossItemsChanged()), idw, SLOT(setLayoutFromText()) );
        connect( text, SIGNAL(phraseRefreshNeeded(int)), idw, SLOT(requestLineRefresh(int)) );
        connect( text, SIGNAL(phrasalGlossChanged(int,TextBit)), this, SLOT(updatePhrasalGloss(int,TextBit)) );

        connect( text, SIGNAL(guiRefreshRequest()), idw, SLOT(resetGui()) );

        mIdwTabs << idw;
        addTab( idw, tab->name() );
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

void TextDisplayWidget::tabChanged(int i)
{
    Q_UNUSED(i);
    //! @todo Apparently this should be removed?
//    if( i != 0 )
    //        mText->setBaselineText( ui->baselineTextEdit->toPlainText() );
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

void TextDisplayWidget::focusGlossLine(int line)
{
    Q_UNUSED(line);
    //! @todo Apparently this should be removed?
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
