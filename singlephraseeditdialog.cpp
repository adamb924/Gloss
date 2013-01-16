#include "singlephraseeditdialog.h"
#include "ui_singlephraseeditdialog.h"

#include "glossdisplaywidget.h"
#include "text.h"

#include <QList>

SinglePhraseEditDialog::SinglePhraseEditDialog(QList<int> lines, Project *project, Text * text, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::SinglePhraseEditDialog)
{
    mLines = lines;
    mProject = project;
    mText = text;

    mGlossDisplayWidget = 0;
    ui->setupUi(this);

    QString label;
    if( mLines.count() > 1 )
    {
        QString lineString;
        for(int i=0; i<mLines.count(); i++)
        {
            lineString.append(QString("%1").arg(i+1));
            if( i != mLines.count() - 1 )
                lineString.append(", ");
        }
    }
    else
    {
        setWindowTitle( tr("%1 - Line %2").arg(text->name()).arg(mLines.first()+0) );
    }

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

    mGlossDisplayWidget = new GlossDisplayWidget( mText, mProject, mLines, this );

    for(int i=0; i<mLines.count(); i++)
        connect( mText->phrases()->at(mLines.at(i)), SIGNAL(phraseChanged()), this, SLOT(accept()) );

    ui->layout->addWidget( mGlossDisplayWidget );
}
