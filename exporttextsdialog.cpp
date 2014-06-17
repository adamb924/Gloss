#include "exporttextsdialog.h"
#include "ui_exporttextsdialog.h"

#include <QFileDialog>

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
