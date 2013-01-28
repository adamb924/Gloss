#include "textdisplaywidget.h"
#include "ui_textdisplaywidget.h"

#include "databaseadapter.h"
#include "text.h"
#include "interlineardisplaywidget.h"

#include <QCloseEvent>
#include <QLabel>

TextDisplayWidget::TextDisplayWidget(Text *text, Project *project, QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TextDisplayWidget)
{
    setAttribute(Qt::WA_DeleteOnClose);

    mText = text;
    mProject = project;

    ui->setupUi(this);
    connect(this,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));

    ui->baselineTextEdit->setWritingSystem(text->baselineWritingSystem());
    ui->baselineTextEdit->setPlainText( text->baselineText() );

    mGloss = new InterlinearDisplayWidget(mProject->dbAdapter()->glossInterlinearLines(), mProject->dbAdapter()->glossPhrasalGlossLines(), mText, mProject, this);
    ui->glossTab->layout()->addWidget(mGloss);

    // text update connections
    connect( text, SIGNAL(baselineTextChanged(QString)), mGloss, SLOT(setLayoutFromText()));

    // line number connections
    connect( ui->baselineTextEdit, SIGNAL(lineNumberChanged(int)), mGloss, SLOT(scrollToLine(int)) );
    connect( mGloss, SIGNAL(lineNumberChanged(int)), ui->baselineTextEdit, SLOT(setLineNumber(int)) );

    mAnalysis = new InterlinearDisplayWidget(mProject->dbAdapter()->analysisInterlinearLines(), mProject->dbAdapter()->analysisPhrasalGlossLines(), mText, mProject, this);
    ui->morphologyTab->layout()->addWidget(mAnalysis);

    connect( text, SIGNAL(baselineTextChanged(QString)), ui->baselineTextEdit, SLOT(setPlainText(QString)) );
    connect( text, SIGNAL(glossItemsChanged()), mAnalysis, SLOT(setLayoutFromText()));

    // line update connections
    connect( text, SIGNAL(phraseRefreshNeeded(int)), mGloss, SLOT(requestLineRefresh(int)) );
    connect( text, SIGNAL(phraseRefreshNeeded(int)), mAnalysis, SLOT(requestLineRefresh(int)));

    setWindowTitle(mText->name());
}

TextDisplayWidget::~TextDisplayWidget()
{
    mProject->closeText(mText);
    delete ui;
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
    setCurrentWidget( ui->glossTab );
    mGloss->scrollToLine(line);
}

