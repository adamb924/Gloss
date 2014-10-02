#include "generictextinputdialog.h"
#include "ui_generictextinputdialog.h"

#include "writingsystem.h"
#include "databaseadapter.h"

#include <QDebug>

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
    ui->writingSystemCombo->hide();

    mWritingSystem = writingSystem;

    ui->textEdit->setWritingSystem( mWritingSystem );
}

GenericTextInputDialog::GenericTextInputDialog(const TextBit & bit, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::GenericTextInputDialog)
{
    ui->setupUi(this);
    ui->writingSystemCombo->hide();

    mWritingSystem = bit.writingSystem();

    ui->textEdit->setTextBit(bit);
}

GenericTextInputDialog::~GenericTextInputDialog()
{
    delete ui;
}

void GenericTextInputDialog::suggestInput(const TextBit &bit)
{
    ui->textEdit->setTextBit( bit );
    ui->textEdit->setSelection( 0, ui->textEdit->text().length() );
}

void GenericTextInputDialog::fillWritingSystems()
{
    for(int i=0; i<mWritingSystems.count(); i++)
        ui->writingSystemCombo->addItem( tr("%1 (%2)").arg(mWritingSystems.at(i).name()).arg(mWritingSystems.at(i).flexString()), i );
}

void GenericTextInputDialog::changeCurrentWritingSystem(int index)
{
    ui->textEdit->setWritingSystem( mWritingSystems.at(index) );
    mWritingSystem = mWritingSystems.at(index);
}

QString GenericTextInputDialog::text() const
{
    return ui->textEdit->text();
}

TextBit GenericTextInputDialog::textBit() const
{
    return ui->textEdit->textBit();
}

WritingSystem GenericTextInputDialog::writingSystem() const
{
    if( ui->writingSystemCombo->isVisible() )
        return mWritingSystems.at( ui->writingSystemCombo->itemData( ui->writingSystemCombo->currentIndex() ).toInt() );
    else
        return mWritingSystem;
}
