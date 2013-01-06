#include "generictextinputdialog.h"
#include "ui_generictextinputdialog.h"

#include "writingsystem.h"
#include "databaseadapter.h"

GenericTextInputDialog::GenericTextInputDialog(DatabaseAdapter *dbAdapter, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GenericTextInputDialog)
{
    ui->setupUi(this);

    mWritingSystems = dbAdapter->writingSystems();

    connect(ui->writingSystemCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeCurrentWritingSystem(int)));

    fillWritingSystems();
}

GenericTextInputDialog::GenericTextInputDialog(const WritingSystem & writingSystem, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::GenericTextInputDialog)
{
    ui->setupUi(this);
    ui->writingSystemCombo->setVisible(false);

    mWritingSystem = writingSystem;

    ui->textEdit->setWritingSystem( mWritingSystem );
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

TextBit GenericTextInputDialog::textBit() const
{
    return TextBit(ui->textEdit->text(), mWritingSystem);
}

WritingSystem GenericTextInputDialog::writingSystem() const
{
    if( ui->writingSystemCombo->isVisible() )
        return mWritingSystems.at( ui->writingSystemCombo->itemData( ui->writingSystemCombo->currentIndex() ).toInt() );
    else
        return mWritingSystem;
}
