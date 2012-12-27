#include "generictextinputdialog.h"
#include "ui_generictextinputdialog.h"

#include "writingsystem.h"
#include "databaseadapter.h"

GenericTextInputDialog::GenericTextInputDialog(DatabaseAdapter *dbAdapter, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GenericTextInputDialog)
{
    ui->setupUi(this);

    mDbAdapter = dbAdapter;

    mWritingSystems = mDbAdapter->writingSystems();

    connect(ui->writingSystemCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeCurrentWritingSystem(int)));

    fillWritingSystems();
}

GenericTextInputDialog::~GenericTextInputDialog()
{
    delete ui;
}

void GenericTextInputDialog::fillWritingSystems()
{
    for(int i=0; i<mWritingSystems.count(); i++)
        ui->writingSystemCombo->addItem( mWritingSystems.at(i).name(), i );
}

void GenericTextInputDialog::changeCurrentWritingSystem(int index)
{
    ui->textEdit->setWritingSystem( mWritingSystems.at(index) );
}

QString GenericTextInputDialog::text() const
{
    return ui->textEdit->text();
}

WritingSystem GenericTextInputDialog::writingSystem() const
{
    return mWritingSystems.at( ui->writingSystemCombo->itemData( ui->writingSystemCombo->currentIndex() ).toInt() );
}
