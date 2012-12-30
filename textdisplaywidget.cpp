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

    mInterlinear = new GlossDisplayWidget(mText, mProject, this);

    connect( ui->baselineTextEdit, SIGNAL(lineNumberChanged(int)), mInterlinear, SLOT(scrollToLine(int)) );
    connect( mInterlinear, SIGNAL(lineNumberChanged(int)), ui->baselineTextEdit, SLOT(setLineNumber(int)) );

    ui->glossTab->layout()->addWidget(mInterlinear);

    mAnalysis = new AnalysisDisplayWidget(mText, mProject, this);
    ui->morphologyTab->layout()->addWidget(mAnalysis);
//    ui->morphologyTab->layout()->addWidget( new QLabel(tr("Different widget")));

    setWindowTitle(mText->name());
}

TextDisplayWidget::~TextDisplayWidget()
{
    mProject->closeText(mText);
    delete ui;
}

void TextDisplayWidget::tabChanged(int i)
{
    if( i == 1 )
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
    mInterlinear->scrollToLine(line);
}

