#include "importplaintextdialog.h"
#include "ui_importplaintextdialog.h"

#include <QFileDialog>
#include <QMessageBox>

ImportPlainTextDialog::ImportPlainTextDialog(const QList<WritingSystem> &writingSystems, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportPlainTextDialog)
{
    ui->setupUi(this);

    ui->writingSystemCombo->setWritingSystems(writingSystems);

    connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(chooseFilename()) );
    connect(ui->delimiterCheckBox, SIGNAL(clicked(bool)), this, SLOT(delimiterChecked(bool)) );
}

ImportPlainTextDialog::~ImportPlainTextDialog()
{
    delete ui;
}

QStringList ImportPlainTextDialog::filenames() const
{
    return ui->filenameEdit->text().split("|");
}

WritingSystem ImportPlainTextDialog::writingSystem() const
{
    return ui->writingSystemCombo->currentWritingSystem();
}

bool ImportPlainTextDialog::customDelimiter() const
{
    return ui->delimiterCheckBox->isChecked();
}

QRegularExpression ImportPlainTextDialog::phraseDelimiter() const
{
    return QRegularExpression(ui->reEdit->text());
}

QRegularExpression ImportPlainTextDialog::paragraphDelimiter() const
{
    return QRegularExpression(ui->paragraphReEdit->text());
}

void ImportPlainTextDialog::chooseFilename()
{
    QFileDialog dlg( this, tr("Select file(s) to import"), QString(), "*.*" );
    dlg.setFileMode(QFileDialog::ExistingFiles);
    if( dlg.exec() )
    {
        ui->filenameEdit->setText( dlg.selectedFiles().join("|") );
    }
}

void ImportPlainTextDialog::delimiterChecked(bool checked)
{
    ui->delimiterLabel->setEnabled(checked);
    ui->reEdit->setEnabled(checked);
    ui->paragraphDelimiterLabel->setEnabled(checked);
    ui->paragraphReEdit->setEnabled(checked);
}

void ImportPlainTextDialog::accept()
{
    QRegularExpression re(ui->reEdit->text());
    if( re.isValid() )
    {
        QDialog::accept();
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tr("That's not a valid regular expression."));
    }
}
