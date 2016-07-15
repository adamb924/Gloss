#include "partofspeechdialog.h"
#include "ui_partofspeechdialog.h"

#include <QSqlDatabase>
#include <QSqlTableModel>

#include "project.h"
#include "databaseadapter.h"

PartOfSpeechDialog::PartOfSpeechDialog(const QString & table, Project * prj, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PartOfSpeechDialog),
    mProject(prj),
    mCurrentRow(-1)
{
    ui->setupUi(this);

    mModel = new QSqlTableModel( this, QSqlDatabase::database(mProject->dbAdapter()->dbFilename()) );
    mModel->setTable(table);
    mModel->setEditStrategy( QSqlTableModel::OnManualSubmit );
    mModel->select();

    ui->listView->setModel(mModel);
    ui->listView->setModelColumn( 2 );

    connect( ui->add, SIGNAL(clicked()), this, SLOT(add()));
    connect( ui->remove, SIGNAL(clicked()), this, SLOT(remove()) );

    connect( ui->listView, SIGNAL(clicked(QModelIndex)), this, SLOT(changeRow(QModelIndex)) );

    /// this creates a little redundancy in execution, but the code is much more elegant
    connect( ui->abbreviation, SIGNAL(editingFinished()), this, SLOT(updateDatabaseRecord()) );
    connect( ui->name, SIGNAL(editingFinished()), this, SLOT(updateDatabaseRecord()) );
    connect( ui->keystroke, SIGNAL(editingFinished()), this, SLOT(validateKeystroke()) );
    connect( ui->keystroke, SIGNAL(editingFinished()), this, SLOT(updateDatabaseRecord()) );
}

PartOfSpeechDialog::~PartOfSpeechDialog()
{
    delete ui;
}

void PartOfSpeechDialog::add()
{
    QSqlRecord r = mModel->record();
    if( !mModel->insertRecord(-1, r) )
    {
        qWarning() << "ClosedVocabularyDialog::add() Could not insert row." << mModel->lastError();
    }

    QModelIndex index = mModel->index( mModel->rowCount()-1, 0);
    ui->listView->setCurrentIndex( index );
    changeRow( index );
    ui->abbreviation->setFocus();
}

void PartOfSpeechDialog::remove()
{
    if( mCurrentRow != -1 )
    {
        mModel->removeRow(mCurrentRow);
        ui->name->setText("");
        ui->abbreviation->setText("");
        ui->keystroke->setText("");

        ui->name->setEnabled(false);
        ui->abbreviation->setEnabled(false);
        ui->keystroke->setEnabled(false);

        mModel->submitAll();
        mModel->select();
    }
}

void PartOfSpeechDialog::changeRow(const QModelIndex &index)
{
    mCurrentRow = index.row();
    QSqlRecord r = mModel->record(mCurrentRow);
    ui->name->setText( r.value("Name").toString() );
    ui->abbreviation->setText( r.value("Abbreviation").toString() );
    ui->keystroke->setText( r.value("KeySequence").toString() );

    ui->name->setEnabled(true);
    ui->abbreviation->setEnabled(true);
    ui->keystroke->setEnabled(true);
}

void PartOfSpeechDialog::updateDatabaseRecord()
{
    if( mCurrentRow < 0 ) return;
    QSqlRecord r = mModel->record(mCurrentRow);
    r.setValue( "Name" , ui->name->text() );
    r.setValue( "Abbreviation" , ui->abbreviation->text() );
    r.setValue( "KeySequence" , ui->keystroke->text() );

    r.setGenerated( "Name" , true );
    r.setGenerated( "Abbreviation" , true );
    r.setGenerated( "KeySequence" , true );

    if( !mModel->setRecord(mCurrentRow, r) )
    {
        qWarning() << "PartOfSpeechDialog::updateDatabaseRecord() Could not set record, row" << mCurrentRow;
    }
}

void PartOfSpeechDialog::validateKeystroke()
{
    ui->keystroke->setText( QKeySequence(ui->keystroke->text()).toString() );
}

void PartOfSpeechDialog::accept()
{
    if( ! mModel->submitAll() )
    {
        qWarning() << "PartOfSpeechDialog::finalizeDatabase()" << mModel->lastError();
    }
    mModel->database().commit();
    QDialog::accept();
}

void PartOfSpeechDialog::reject()
{
    mModel->database().rollback();
    QDialog::reject();
}
