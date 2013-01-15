#include "textdisplaywidget.h"
#include "ui_textdisplaywidget.h"

#include "databaseadapter.h"
#include "text.h"
#include "glossdisplaywidget.h"
#include "analysisdisplaywidget.h"

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

    mGloss = new GlossDisplayWidget(mText, mProject, this);

    connect( ui->baselineTextEdit, SIGNAL(lineNumberChanged(int)), mGloss, SLOT(scrollToLine(int)) );
    connect( mGloss, SIGNAL(lineNumberChanged(int)), ui->baselineTextEdit, SLOT(setLineNumber(int)) );
    ui->glossTab->layout()->addWidget(mGloss);

    mAnalysis = new AnalysisDisplayWidget(mText, mProject, this);
    ui->morphologyTab->layout()->addWidget(mAnalysis);


    connect( text, SIGNAL(baselineTextChanged()), mGloss, SLOT(setLayoutAsAppropriate()));
    connect( text, SIGNAL(glossItemsChanged()), mAnalysis, SLOT(setLayoutAsAppropriate()));

    setWindowTitle(mText->name());
}

TextDisplayWidget::~TextDisplayWidget()
{
    mProject->closeText(mText);
    delete ui;
}

void TextDisplayWidget::tabChanged(int i)
{
    if( i == 1 || i == 2 ) // the gloss tab or the morphology tab
        mText->setBaselineText( ui->baselineTextEdit->toPlainText() );
}

void TextDisplayWidget::closeEvent(QCloseEvent *event)
{
    mText->saveText();
    event->accept();
}

void TextDisplayWidget::focusGlossLine(int line)
{
    setCurrentWidget( ui->glossTab );
    mGloss->scrollToLine(line);
}

