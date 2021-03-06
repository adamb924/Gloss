#include "importflextextdialog.h"
#include "ui_importflextextdialog.h"

#include <QtWidgets>
#include <QtXml>
#include <QXmlQuery>
#include <QXmlResultItems>
#include <QStringList>

#include "writingsystem.h"
#include "databaseadapter.h"
#include "messagehandler.h"

ImportFlexTextDialog::ImportFlexTextDialog(DatabaseAdapter *dbAdapter, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::ImportFlexTextDialog)
{
    mDbAdapter = dbAdapter;

    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    connect(ui->choose, SIGNAL(clicked()), this, SLOT(chooseFile()));
    connect(ui->filename, SIGNAL(textChanged(QString)), this, SLOT(fillDataFromFlexText()));

    if( !chooseFile() )
        QTimer::singleShot(0, this, SLOT(close()));

    setWindowTitle(tr("Import Flex Text"));
}

ImportFlexTextDialog::~ImportFlexTextDialog()
{
    delete ui;
}

bool ImportFlexTextDialog::chooseFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open") , QString(), "FlexText (*.flextext)" );
    if( filename.isNull() )
    {
        return false;
    }
    else
    {
        ui->filename->setText(filename);
        return true;
    }
}

void ImportFlexTextDialog::fillDataFromFlexText()
{
    QString filename = ui->filename->text();
    if( filename.isNull() || !QFile::exists(filename) )
    {
        QMessageBox::information(this,tr("Error reading file"), tr("The file you've chosen appears not to exist."));
        disable();
        return;
    }

    QXmlQuery query(QXmlQuery::XQuery10);
    query.setMessageHandler(new MessageHandler("ImportFlexTextDialog::fillDataFromFlexText", this));
    query.bindVariable("path", QVariant(QUrl::fromLocalFile(filename).path(QUrl::FullyEncoded)));
    query.setQuery("declare namespace abg = \"http://www.adambaker.org/gloss.php\"; "
                   "declare variable $path external; "
                   "for $x in doc($path)/document/interlinear-text/languages/language return string-join( ( string($x/@lang),string($x/@font),string($x/@RightToLeft) ) , ',')");
    if (query.isValid())
    {
        QStringList result;
        query.evaluateTo(&result);

        for(int i=0; i<result.count(); i++)
        {
            QStringList items = result.at(i).split(",");
            if( items.count() == 3)
            {
                if( !mDbAdapter->writingSystemExists(items.at(0)) )
                    mDbAdapter->addWritingSystem(items.at(0), items.at(1), items.at(2) == "true" ? Qt::RightToLeft : Qt::LeftToRight );
                WritingSystem ws = mDbAdapter->writingSystem(items.at(0));
                ui->baselineWritingSystem->addItem( ws.name() , items.at(0) );
            }
        }
        enable();
    }
    else
    {
        // report errors here
        QMessageBox::information(this,tr("Error reading file"), tr("Invalid query -- my bad."));
    }
}

void ImportFlexTextDialog::enable()
{
    ui->baselineWritingSystem->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void ImportFlexTextDialog::disable()
{
    ui->baselineWritingSystem->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

QString ImportFlexTextDialog::filename() const
{
    return ui->filename->text();
}

QString ImportFlexTextDialog::writingSystem() const
{
    return ui->baselineWritingSystem->itemData(ui->baselineWritingSystem->currentIndex()).toString();
}

