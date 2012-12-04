#include "textdisplaywidget.h"
#include "ui_textdisplaywidget.h"

#include "interlineardisplaywidget.h"
#include "databaseadapter.h"
#include "text.h"

#include <QScrollArea>

TextDisplayWidget::TextDisplayWidget(Text *text, Project *project, QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TextDisplayWidget)
{
    setAttribute(Qt::WA_DeleteOnClose);

    mText = text;
    mProject = project;

    ui->setupUi(this);
    connect(this,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));

    mInterlinear = new InterlinearDisplayWidget(mText, mProject, this);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setBackgroundRole(QPalette::Light);
    scrollArea->setWidget(mInterlinear);

    ui->glossTab->layout()->addWidget(scrollArea);

    setWindowTitle(mText->name());
}

TextDisplayWidget::~TextDisplayWidget()
{
    delete ui;
}

void TextDisplayWidget::tabChanged(int i)
{
    // TODO eventually we'll need something finer-grained here
    if( i == 1 )
    {
        mText->setBaselineText( ui->plainTextEdit->toPlainText() );
    }
}

void TextDisplayWidget::closeEvent(QCloseEvent *event)
{
    mText->saveText(mProject->getTempDir());
    event->accept();
}
