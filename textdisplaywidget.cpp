#include "textdisplaywidget.h"
#include "ui_textdisplaywidget.h"

#include "interlineardisplaywidget.h"
#include "databaseadapter.h"

TextDisplayWidget::TextDisplayWidget(Project *project, QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TextDisplayWidget)
{
    setAttribute(Qt::WA_DeleteOnClose);

    mProject = project;

    // TODO this will need to be modifiable
    mBaselineMode = Project::Orthographic;

    // TODO this too need to be modifiable
    mWritingSystem = mProject->dbAdapter()->writingSystem("wbl-Arab-AF");

    ui->setupUi(this);
    connect(this,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));

    mInterlinear = new InterlinearDisplayWidget(mWritingSystem, mBaselineMode, mProject, this);
    ui->glossTab->layout()->addWidget(mInterlinear);
}

TextDisplayWidget::~TextDisplayWidget()
{
    delete ui;
    delete mWritingSystem;
}

void TextDisplayWidget::tabChanged(int i)
{
    // TODO eventually we'll need something finer-grained here
    if( i == 1 )
    {
        mInterlinear->setText( ui->plainTextEdit->toPlainText() );
    }
}

Project::BaselineMode TextDisplayWidget::textBaselineMode() const
{
    return mBaselineMode;
}

WritingSystem* TextDisplayWidget::writingSystem() const
{
    return mWritingSystem;
}

void TextDisplayWidget::setWritingSystem(WritingSystem *ws)
{
    mWritingSystem = ws;
}
