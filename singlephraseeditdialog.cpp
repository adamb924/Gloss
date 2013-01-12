#include "singlephraseeditdialog.h"
#include "ui_singlephraseeditdialog.h"

#include "phrasedisplaywidget.h"
#include "phrase.h"
#include "databaseadapter.h"
#include "text.h"

SinglePhraseEditDialog::SinglePhraseEditDialog(int index, Phrase *phrase, Text * text, DatabaseAdapter *dbAdapter,  QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SinglePhraseEditDialog)
{
    ui->setupUi(this);
    ui->scrollArea->setBackgroundRole(QPalette::Light);
    ui->scrollArea->setWidget( new PhraseDisplayWidget(index, phrase, text, 0, dbAdapter) );

    setWindowTitle( tr("%1 - Line %2").arg(text->name()).arg(index+1) );
}

SinglePhraseEditDialog::~SinglePhraseEditDialog()
{
    delete ui;
}
