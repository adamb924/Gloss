#include "importflextextdialog.h"
#include "ui_importflextextdialog.h"

#include <QtGui>
#include <QtXml>
#include <QXmlQuery>
#include <QXmlResultItems>
#include <QStringList>

#include "writingsystem.h"
#include "project.h"
#include "databaseadapter.h"

ImportFlexTextDialog::ImportFlexTextDialog(Project *project, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::ImportFlexTextDialog)
{
    mProject = project;

    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    connect(ui->choose, SIGNAL(clicked()), this, SLOT(chooseFile()));
    connect(ui->filename, SIGNAL(textChanged(QString)), this, SLOT(fillDataFromFlexText()));
}

ImportFlexTextDialog::~ImportFlexTextDialog()
{
    delete ui;
}

void ImportFlexTextDialog::chooseFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open") , QString(), "FlexText (*.flextext)" );
    if( !filename.isNull() )
        ui->filename->setText(filename);
}

void ImportFlexTextDialog::fillDataFromFlexText()
{
    QString filename = ui->filename->text();
    if( filename.isNull() || !QFile::exists(filename) )
    {
        disable();
        return;
    }

    QDomDocument document;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        disable();
        return;
    }

    if (!document.setContent(&file))
    {
        file.close();
        disable();
        return;
    }
    file.close();

    QXmlQuery query(QXmlQuery::XQuery10);
    query.setFocus(QUrl(filename));
    query.setQuery("for $x in /document/interlinear-text/languages/language return string-join( ( string($x/@lang),string($x/@font),string($x/@RightToLeft) ) , ',')");
    if (query.isValid()) {
        QStringList result;
        query.evaluateTo(&result);

        for(int i=0; i<result.count(); i++)
        {
            QStringList items = result.at(i).split(",");
            if( items.count() == 3)
            {
                if( !mProject->dbAdapter()->writingSystemExists(items.at(0)) )
                {
                    mProject->dbAdapter()->addWritingSystem(items.at(0), items.at(1), items.at(2) == "true" ? Qt::RightToLeft : Qt::LeftToRight );
                }
                ui->baselineWritingSystem->addItem( items.at(0) , items.at(0) );
            }
        }
        enable();
    }
}

void ImportFlexTextDialog::enable()
{
    ui->baselineMode->setEnabled(true);
    ui->baselineWritingSystem->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void ImportFlexTextDialog::disable()
{
    ui->baselineMode->setEnabled(false);
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

Text::BaselineMode ImportFlexTextDialog::baselineMode() const
{
    return (Text::BaselineMode)ui->baselineMode->itemData(ui->baselineMode->currentIndex()).toInt();
}
