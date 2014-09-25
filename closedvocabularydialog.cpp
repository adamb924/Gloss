#include "closedvocabularydialog.h"
#include "ui_closedvocabularydialog.h"

#include <QSqlDatabase>
#include <QSqlTableModel>

#include "project.h"
#include "databaseadapter.h"

ClosedVocabularyDialog::ClosedVocabularyDialog(Project * prj, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClosedVocabularyDialog),
    mProject(prj),
    mCurrentRow(-1)
{
    ui->setupUi(this);

    mModel = new QSqlTableModel( this, QSqlDatabase::database(mProject->dbAdapter()->dbFilename()) );
    mModel->setTable("SyntacticConstituents");
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
    connect( ui->parentCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateDatabaseRecord()) );
    connect( ui->parentComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDatabaseRecord()) );

    connect( ui->parentCheckBox, SIGNAL(toggled(bool)), ui->parentComboBox, SLOT(setEnabled(bool)) );
    ui->parentCheckBox->setEnabled(false);
    ui->parentComboBox->setEnabled(false);

    ui->parentComboBox->setModel(mModel);
    ui->parentComboBox->setModelColumn( 2 );
}

ClosedVocabularyDialog::~ClosedVocabularyDialog()
{
    delete ui;
}

void ClosedVocabularyDialog::add()
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

void ClosedVocabularyDialog::remove()
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
        ui->parentCheckBox->setEnabled(false);

        mModel->submitAll();
        mModel->select();
    }
}

void ClosedVocabularyDialog::changeRow(const QModelIndex &index)
{
    mCurrentRow = index.row();
    QSqlRecord r = mModel->record(mCurrentRow);
    ui->name->setText( r.value("Name").toString() );
    ui->abbreviation->setText( r.value("Abbreviation").toString() );
    ui->keystroke->setText( r.value("KeySequence").toString() );

    if( r.value("AutomaticParent").toString().isEmpty() )
    {
        ui->parentCheckBox->setChecked(false);
    }
    else
    {
        ui->parentCheckBox->setChecked(true);
        ui->parentComboBox->setCurrentText( r.value("AutomaticParent").toString() );
    }

    ui->name->setEnabled(true);
    ui->abbreviation->setEnabled(true);
    ui->keystroke->setEnabled(true);
    ui->parentCheckBox->setEnabled(true);
}

void ClosedVocabularyDialog::updateDatabaseRecord()
{
    if( mCurrentRow < 0 ) return;
    QSqlRecord r = mModel->record(mCurrentRow);
    r.setValue( "Name" , ui->name->text() );
    r.setValue( "Abbreviation" , ui->abbreviation->text() );
    r.setValue( "KeySequence" , ui->keystroke->text() );
    if( ui->parentCheckBox->isChecked() )
    {
        r.setValue( "AutomaticParent" , ui->parentComboBox->currentText() );
    }
    else
    {
        r.setValue( "AutomaticParent" , "" );
    }

    r.setGenerated( "Name" , true );
    r.setGenerated( "Abbreviation" , true );
    r.setGenerated( "KeySequence" , true );
    r.setGenerated( "AutomaticParent" , true );

    if( !mModel->setRecord(mCurrentRow, r) )
    {
        qWarning() << "ClosedVocabularyDialog::updateDatabaseRecord() Could not set record, row" << mCurrentRow;
    }
}

void ClosedVocabularyDialog::validateKeystroke()
{
    ui->keystroke->setText( QKeySequence(ui->keystroke->text()).toString() );
}

void ClosedVocabularyDialog::accept()
{
    if( ! mModel->submitAll() )
    {
        qWarning() << "ClosedVocabularyDialog::finalizeDatabase()" << mModel->lastError();
    }
    mModel->database().commit();
    QDialog::accept();
}

void ClosedVocabularyDialog::reject()
{
    mModel->database().rollback();
    QDialog::reject();
}
