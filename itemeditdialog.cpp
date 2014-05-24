#include "itemeditdialog.h"
#include "ui_itemeditdialog.h"

#include "interlinearitemtype.h"

ItemEditDialog::ItemEditDialog(const QList<WritingSystem> & writingSystems, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ItemEditDialog)
{
    ui->setupUi(this);

    ui->itemTypeCombo->addItem( tr("Immutable Text") , InterlinearItemType::ImmutableText );
    ui->itemTypeCombo->addItem( tr("Immutable Gloss") , InterlinearItemType::ImmutableGloss );
    ui->itemTypeCombo->addItem( tr("Text") , InterlinearItemType::Text );
    ui->itemTypeCombo->addItem( tr("Gloss") , InterlinearItemType::Gloss );
    ui->itemTypeCombo->addItem( tr("Analysis") , InterlinearItemType::Analysis );

    ui->writingSystemCombo->setWritingSystems(writingSystems);
}

ItemEditDialog::~ItemEditDialog()
{
    delete ui;
}

InterlinearItemType ItemEditDialog::type() const
{
    return InterlinearItemType( (InterlinearItemType::LineType)ui->itemTypeCombo->currentData().toULongLong(), ui->writingSystemCombo->currentWritingSystem() );
}

void ItemEditDialog::setType(InterlinearItemType type)
{
    for(int i=0; i<ui->itemTypeCombo->count(); i++)
    {
        if(ui->itemTypeCombo->itemData(i) == type.type() )
        {
            ui->itemTypeCombo->setCurrentIndex(i);
        }
    }
    ui->writingSystemCombo->setCurrentWritingSystem(type.writingSystem());
}
