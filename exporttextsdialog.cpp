#include "exporttextsdialog.h"
#include "ui_exporttextsdialog.h"

#include <QFileDialog>
#include <QMenu>

ExportTextsDialog::ExportTextsDialog(QStringList textNames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportTextsDialog)
{
    ui->setupUi(this);

    connect(ui->selectAll, SIGNAL(clicked()), this, SLOT(selectAll()));
    connect(ui->selectNone, SIGNAL(clicked()), this, SLOT(selectNone()));
    connect(ui->browse, SIGNAL(clicked()), this, SLOT(chooseFolder()));

    for( int i=0; i<textNames.count(); i++ )
    {
        QStandardItem *item = new QStandardItem(textNames.at(i));
        item->setCheckable( true );
        item->setCheckState( Qt::Checked );
        mTextNamesModel.setItem( i, item );
    }

    ui->listView->setModel(&mTextNamesModel);

    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenu(QPoint)) );
}

ExportTextsDialog::~ExportTextsDialog()
{
    delete ui;
}

QStringList ExportTextsDialog::textNames() const
{
    QStringList retVal;
    for(int i=0; i<mTextNamesModel.rowCount(); i++)
    {
        if( mTextNamesModel.item(i)->checkState() == Qt::Checked )
        {
            retVal << mTextNamesModel.item(i)->text();
        }
    }
    return retVal;
}

bool ExportTextsDialog::includeGlossNamespace() const
{
    return ui->abgNamespaceCheckbox->isChecked();
}

QString ExportTextsDialog::destinationFolder() const
{
    return ui->destinationFolder->text();
}

void ExportTextsDialog::chooseFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Destination Folder"),
                                                    "",
                                                    QFileDialog::ShowDirsOnly);
    if( !dir.isEmpty() )
    {
        ui->destinationFolder->setText(dir);
    }
}

void ExportTextsDialog::selectAll()
{
    for(int i=0; i<mTextNamesModel.rowCount(); i++)
    {
        mTextNamesModel.item(i)->setCheckState( Qt::Checked );
    }
}

void ExportTextsDialog::selectNone()
{
    for(int i=0; i<mTextNamesModel.rowCount(); i++)
    {
        mTextNamesModel.item(i)->setCheckState( Qt::Unchecked );
    }
}

void ExportTextsDialog::onCustomContextMenu(const QPoint &point)
{
    QMenu *menu = new QMenu(this);

    menu->addAction(tr("Check selected"), this, SLOT(checkSelected()));
    menu->addAction(tr("Uncheck selected"), this, SLOT(uncheckSelected()));

    menu->addSeparator();

    menu->addAction(tr("Check all"), this, SLOT(selectAll()));
    menu->addAction(tr("Clear all checks"), this, SLOT(selectNone()));

    menu->exec( ui->listView->mapToGlobal( point ) );

}

void ExportTextsDialog::checkSelected()
{
    QModelIndexList selection = ui->listView->selectionModel()->selectedIndexes();
    foreach(QModelIndex index, selection)
    {
        mTextNamesModel.itemFromIndex(index)->setCheckState( Qt::Checked );
    }
}

void ExportTextsDialog::uncheckSelected()
{
    QModelIndexList selection = ui->listView->selectionModel()->selectedIndexes();
    foreach(QModelIndex index, selection)
    {
        mTextNamesModel.itemFromIndex(index)->setCheckState( Qt::Unchecked );
    }
}
