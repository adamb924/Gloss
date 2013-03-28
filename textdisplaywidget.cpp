#include "textdisplaywidget.h"
#include "ui_textdisplaywidget.h"

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

    if( view->showBaselineTextTab() )
        setupBaselineTab();

    for(int i=0; i<view->tabs()->count(); i++)
    {
        Tab tab = view->tabs()->at(i);
        InterlinearDisplayWidget * idw = new InterlinearDisplayWidget( tab.interlinearLines(), tab.phrasalGlossLines(), mText, mProject, this);
        idw->setFocus(foci);
        idw->setLines(lines);

        connect( text, SIGNAL(baselineTextChanged(QString)), idw, SLOT(setLayoutFromText()) );
        connect( text, SIGNAL(glossItemsChanged()), idw, SLOT(setLayoutFromText()) );
        if( view->showBaselineTextTab() )
            connect( mBaselineTextEdit, SIGNAL(lineNumberChanged(int)), idw, SLOT(scrollToLine(int)) );
        connect( text, SIGNAL(phraseRefreshNeeded(int)), idw, SLOT(requestLineRefresh(int)) );
        if( view->showBaselineTextTab() )
            connect( idw, SIGNAL(lineNumberChanged(int)), mBaselineTextEdit, SLOT(setLineNumber(int)) );
        // the above is a bit odd because I'm not sure whether the order is important

        mIdwTabs << idw;
        addTab( idw, tab.name() );
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

void TextDisplayWidget::setupBaselineTab()
{
    mBaselineTextEdit = new LingTextEdit(this);
    mBaselineTextEdit->setWritingSystem( mText->baselineWritingSystem());
    mBaselineTextEdit->setPlainText( mText->baselineText() );
    connect( mText, SIGNAL(baselineTextChanged(QString)), mBaselineTextEdit, SLOT(setPlainText(QString)) );
    addTab( mBaselineTextEdit, tr("Baseline") );
}

void TextDisplayWidget::tabChanged(int i)
{
//    if( i != 0 )
//        mText->setBaselineText( ui->baselineTextEdit->toPlainText() );
}

void TextDisplayWidget::closeEvent(QCloseEvent *event)
{
    saveText();
    event->accept();
}

void TextDisplayWidget::focusGlossLine(int line)
{
}

void TextDisplayWidget::setLines(const QList<int> & lines)
{
    for(int i=0; i<mIdwTabs.count(); i++)
        mIdwTabs.at(i)->setLines(lines);
}

void TextDisplayWidget::saveText()
{
    mText->saveText(false);
}
