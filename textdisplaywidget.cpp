#include "textdisplaywidget.h"
#include "ui_textdisplaywidget.h"

#include "interlineardisplaywidget.h"
#include "databaseadapter.h"

TextDisplayWidget::TextDisplayWidget(const TextInfo & info, Project *project, QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TextDisplayWidget)
{
    setAttribute(Qt::WA_DeleteOnClose);

    mTextInfo = info;
    mProject = project;

    ui->setupUi(this);
    connect(this,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));

    mInterlinear = new InterlinearDisplayWidget(mTextInfo.writingSystem(), mTextInfo.baselineMode(), mProject, this);
    ui->glossTab->layout()->addWidget(mInterlinear);

    this->setWindowTitle(info.name());
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
        mInterlinear->setText( ui->plainTextEdit->toPlainText() );
    }
}
