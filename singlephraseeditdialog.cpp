#include "singlephraseeditdialog.h"
#include "ui_singlephraseeditdialog.h"

#include "glossdisplaywidget.h"
#include "text.h"

SinglePhraseEditDialog::SinglePhraseEditDialog(int lineNumber, Project *project, Text * text, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SinglePhraseEditDialog)
{
    mLineNumber = lineNumber;
    mProject = project;
    mText = text;

    mGlossDisplayWidget = 0;
    ui->setupUi(this);

    setWindowTitle( tr("%1 - Line %2").arg(text->name()).arg(lineNumber+1) );

    refreshLayout();
}

SinglePhraseEditDialog::~SinglePhraseEditDialog()
{
    delete ui;
}

void SinglePhraseEditDialog::refreshLayout()
{
//    while( ui->layout->count() > 0 )
//    {
//        QLayoutItem * item = ui->layout->takeAt(0);
//        delete item->widget();
//        delete item;
//    }
//    if( mGlossDisplayWidget != 0 )
//        delete mGlossDisplayWidget;

    QList<int> lines;
    lines << mLineNumber;
    mGlossDisplayWidget = new GlossDisplayWidget( mText, mProject, lines, this );

//    connect( mText->phrases()->at(mLineNumber), SIGNAL(phraseChanged()), this, SLOT(refreshLayout()) );
    connect( mText->phrases()->at(mLineNumber), SIGNAL(phraseChanged()), this, SLOT(accept()) );

    ui->layout->addWidget( mGlossDisplayWidget );
}
