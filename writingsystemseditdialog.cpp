#include "writingsystemseditdialog.h"
#include "ui_writingsystemseditdialog.h"

#include "project.h"
#include "databaseadapter.h"

#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QFileDialog>
#include <QFontDialog>

WritingSystemsEditDialog::WritingSystemsEditDialog(Project *prj, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WritingSystemsEditDialog),
    mCurrentRow(-1)
{
    ui->setupUi(this);

    mModel = new QSqlTableModel( this, QSqlDatabase::database(prj->dbAdapter()->dbFilename()) );

    mModel->database().transaction();

    mModel->setTable("WritingSystems");
    mModel->setEditStrategy( QSqlTableModel::OnRowChange );
    mModel->select();

    ui->listView->setModel(mModel);
    ui->listView->setModelColumn( 1 );

    connect( ui->addButton, SIGNAL(clicked()), this, SLOT(add()));
    connect( ui->removeButton, SIGNAL(clicked()), this, SLOT(remove()) );

    connect( ui->fontBrowseButton, SIGNAL(clicked()), this, SLOT(browseFont()) );
    connect( ui->keyboardBrowseButton, SIGNAL(clicked()), this, SLOT(browseKeyboard()) );

    connect( ui->listView, SIGNAL(clicked(QModelIndex)), this, SLOT(changeRow(QModelIndex)) );

    connect( ui->nameLineEdit, SIGNAL(editingFinished()), this, SLOT(updateDatabaseRecord()) );
    connect( ui->abbreviationLineEdit, SIGNAL(editingFinished()), this, SLOT(updateDatabaseRecord()) );
    connect( ui->flexStringLineEdit, SIGNAL(editingFinished()), this, SLOT(updateDatabaseRecord()) );
    connect( ui->keyboardSwitchFileLineEdit, SIGNAL(editingFinished()), this, SLOT(updateDatabaseRecord()) );
    connect( ui->fontFamilyLineEdit, SIGNAL(editingFinished()), this, SLOT(updateDatabaseRecord()) );
    connect( ui->fontSizeLineEdit, SIGNAL(editingFinished()), this, SLOT(updateDatabaseRecord()) );
    connect( ui->textDirectionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDatabaseRecord()) );

    connect( ui->keyboardSwitchFileLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateDatabaseRecord()) );
    connect( ui->fontFamilyLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateDatabaseRecord()) );
    connect( ui->fontSizeLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateDatabaseRecord()) );

    QRegularExpression rx("([a-zA-Z]|-)+");
    QValidator *validator = new QRegularExpressionValidator(rx, this);
    ui->flexStringLineEdit->setValidator(validator);

    ui->fontSizeLineEdit->setValidator(new QIntValidator(1,200));
}

WritingSystemsEditDialog::~WritingSystemsEditDialog()
{
    delete ui;
}


void WritingSystemsEditDialog::add()
{
    QSqlRecord r = mModel->record();
    if( !mModel->insertRecord(-1, r) )
    {
        qWarning() << "WritingSystemsEditDialog::add() Could not insert row." << mModel->lastError();
    }

    QModelIndex index = mModel->index( mModel->rowCount()-1, 0);
    ui->listView->setCurrentIndex( index );
    changeRow( index );
    ui->nameLineEdit->setFocus();
}

void WritingSystemsEditDialog::remove()
{
    if( mCurrentRow != -1 )
    {
        if( ! mModel->removeRow(mCurrentRow) )
        {
            qDebug() << "WritingSystemsEditDialog::remove()" << mModel->lastError();
        }
        ui->nameLineEdit->setText("");
        ui->abbreviationLineEdit->setText("");
        ui->flexStringLineEdit->setText("");
        ui->keyboardSwitchFileLineEdit->setText("");
        ui->fontFamilyLineEdit->setText("");
        ui->fontSizeLineEdit->setText("");

        ui->nameLineEdit->setEnabled(false);
        ui->abbreviationLineEdit->setEnabled(false);
        ui->flexStringLineEdit->setEnabled(false);
        ui->keyboardSwitchFileLineEdit->setEnabled(false);
        ui->fontFamilyLineEdit->setEnabled(false);
        ui->fontSizeLineEdit->setEnabled(false);
        ui->fontBrowseButton->setEnabled(false);
        ui->keyboardBrowseButton->setEnabled(false);
        ui->textDirectionComboBox->setEnabled(false);

        mModel->submitAll();
        mModel->select();
    }
}

void WritingSystemsEditDialog::changeRow(const QModelIndex &index)
{
    mCurrentRow = index.row();
    QSqlRecord r = mModel->record(mCurrentRow);
    ui->nameLineEdit->setText( r.value("Name").toString() );
    ui->abbreviationLineEdit->setText( r.value("Abbreviation").toString() );
    ui->flexStringLineEdit->setText( r.value("FlexString").toString() );
    ui->keyboardSwitchFileLineEdit->setText( r.value("KeyboardCommand").toString() );
    ui->fontFamilyLineEdit->setText( r.value("FontFamily").toString() );
    ui->fontSizeLineEdit->setText( r.value("FontSize").toString() );
    ui->textDirectionComboBox->setCurrentText( r.value("Direction").toInt() == Qt::LeftToRight ? tr("Left-to-right") : tr("Right-to-left") );

    ui->nameLineEdit->setEnabled(true);
    ui->abbreviationLineEdit->setEnabled(true);
    ui->flexStringLineEdit->setEnabled(true);
    ui->keyboardSwitchFileLineEdit->setEnabled(true);
    ui->fontFamilyLineEdit->setEnabled(true);
    ui->fontSizeLineEdit->setEnabled(true);
    ui->fontBrowseButton->setEnabled(true);
    ui->keyboardBrowseButton->setEnabled(true);
    ui->textDirectionComboBox->setEnabled(true);
}

void WritingSystemsEditDialog::updateDatabaseRecord()
{
    if( mCurrentRow < 0 ) return;
    QSqlRecord r = mModel->record(mCurrentRow);

    r.setValue( "Name" , ui->nameLineEdit->text() );
    r.setValue( "Abbreviation" , ui->abbreviationLineEdit->text() );
    r.setValue( "FlexString" , ui->flexStringLineEdit->text() );
    r.setValue( "KeyboardCommand" , ui->keyboardSwitchFileLineEdit->text() );
    r.setValue( "FontFamily" , ui->fontFamilyLineEdit->text() );
    r.setValue( "FontSize" , ui->fontSizeLineEdit->text() );
    r.setValue( "Direction" , ui->textDirectionComboBox->currentText() == tr("Left-to-right") ? Qt::LeftToRight : Qt::RightToLeft );

    r.setGenerated( "Name" , true );
    r.setGenerated( "Abbreviation" , true );
    r.setGenerated( "FlexString" , true );
    r.setGenerated( "KeyboardCommand" , true );
    r.setGenerated( "FontFamily" , true );
    r.setGenerated( "FontSize" , true );
    r.setGenerated( "Direction" , true );

    if( !mModel->setRecord(mCurrentRow, r) )
    {
        qWarning() << "WritingSystemsEditDialog::updateDatabaseRecord() Could not set record, row" << mCurrentRow << mModel->lastError();
    }
}

void WritingSystemsEditDialog::accept()
{
    mModel->database().commit();
    QDialog::accept();
}

void WritingSystemsEditDialog::reject()
{
    mModel->database().rollback();
    QDialog::reject();
}

void WritingSystemsEditDialog::browseKeyboard()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Select file"), "", tr("Any (Executable) File (*.*)"));
    if( !filename.isEmpty() )
    {
        ui->keyboardSwitchFileLineEdit->setText(filename);
        updateDatabaseRecord();
    }
}

void WritingSystemsEditDialog::browseFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont( ui->fontFamilyLineEdit->text(), ui->fontSizeLineEdit->text().toInt() ), this);
    if(ok)
    {
        ui->fontFamilyLineEdit->setText( font.family() );
        ui->fontSizeLineEdit->setText( tr("%1").arg(font.pointSize()) );
        updateDatabaseRecord();
    }
}
