#include "textdisplaywidget.h"
#include "ui_textdisplaywidget.h"

#include "databaseadapter.h"
#include "text.h"
#include "interlineardisplaywidget.h"

#include <QCloseEvent>
#include <QLabel>
#include <QMessageBox>

TextDisplayWidget::TextDisplayWidget(Text *text, Project *project, View::Type type, const QList<Focus> & foci, QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TextDisplayWidget)
{
    setAttribute(Qt::WA_DeleteOnClose);

    mText = text;
    mProject = project;
    const View *view = mProject->view(type);

    if( view == 0 )
    {
        QMessageBox::critical(this, tr("Error"), tr("There is no valid view available. Something is likely wrong with your configuration.xml file."));
    }

    ui->setupUi(this);
    connect(this,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));

    setupBaselineTab();

    for(int i=0; i<view->tabs()->count(); i++)
    {
        Tab tab = view->tabs()->at(i);
        InterlinearDisplayWidget * idw = new InterlinearDisplayWidget( tab.interlinearLines(), tab.phrasalGlossLines(), mText, mProject, this);
        idw->setFocus(foci);
        idw->setLayoutFromText();

        connect( text, SIGNAL(baselineTextChanged(QString)), idw, SLOT(setLayoutFromText()) );
        connect( text, SIGNAL(glossItemsChanged()), idw, SLOT(setLayoutFromText()) );
        connect( ui->baselineTextEdit, SIGNAL(lineNumberChanged(int)), idw, SLOT(scrollToLine(int)) );
        connect( text, SIGNAL(phraseRefreshNeeded(int)), idw, SLOT(requestLineRefresh(int)) );
        connect( idw, SIGNAL(lineNumberChanged(int)), ui->baselineTextEdit, SLOT(setLineNumber(int)) );

        mIdwTabs << idw;
        addTab( idw, tab.name() );
    }

    setWindowTitle(mText->name());
}

TextDisplayWidget::~TextDisplayWidget()
{
    mProject->closeText(mText);
    delete ui;
}

void TextDisplayWidget::setupBaselineTab()
{
    ui->baselineTextEdit->setWritingSystem( mText->baselineWritingSystem());
    ui->baselineTextEdit->setPlainText( mText->baselineText() );
    connect( mText, SIGNAL(baselineTextChanged(QString)), ui->baselineTextEdit, SLOT(setPlainText(QString)) );
}

void TextDisplayWidget::tabChanged(int i)
{
    if( i != 0 )
        mText->setBaselineText( ui->baselineTextEdit->toPlainText() );
}

void TextDisplayWidget::closeEvent(QCloseEvent *event)
{
    mText->saveText(false);
    event->accept();
}

void TextDisplayWidget::focusGlossLine(int line)
{
//    setCurrentWidget( ui->glossTab );
    mGloss->scrollToLine(line);
}
