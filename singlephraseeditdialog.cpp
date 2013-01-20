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

    ui->setupUi(this);

    setTitle();

    ui->layout->addWidget( new GlossDisplayWidget( mText, mProject, mLines, this ) );
}

SinglePhraseEditDialog::~SinglePhraseEditDialog()
{
    delete ui;
}

void SinglePhraseEditDialog::setTitle()
{
    if( mLines.count() > 1 )
    {
        QString lineString;
        for(int i=0; i<mLines.count(); i++)
        {
            lineString.append(QString("%1").arg( mLines.at(i)+1 ));
            if( i != mLines.count() - 1 )
                lineString.append(", ");
        }
        setWindowTitle( tr("%1 - Lines %2").arg(mText->name()).arg(lineString) );
    }
    else
    {
        setWindowTitle( tr("%1 - Line %2").arg(mText->name()).arg(mLines.first()+1) );
    }
}
