#include "closedvocabularydialog.h"
#include "ui_closedvocabularydialog.h"

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

    mDatabase = QSqlDatabase::database(mProject->dbAdapter()->dbFilename());

    mModel = new QSqlTableModel( this, mDatabase );
    mModel->setTable("SyntacticConstituents");
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

ClosedVocabularyDialog::~ClosedVocabularyDialog()
{
    delete ui;
}

void ClosedVocabularyDialog::add()
{
    mModel->insertRow(0);
    QModelIndex index = mModel->index(0,2);
    ui->listView->setCurrentIndex( index );
    changeRow(index);
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
    ui->name->setEnabled(true);
    ui->abbreviation->setEnabled(true);
    ui->keystroke->setEnabled(true);
}

void ClosedVocabularyDialog::updateDatabaseRecord()
{
    QSqlRecord r = mModel->record(mCurrentRow);
    r.setValue( "Name" , ui->name->text() );
    r.setValue( "Abbreviation" , ui->abbreviation->text() );
    r.setValue( "KeySequence" , ui->keystroke->text() );
    mModel->setRecord(mCurrentRow, r);
}

void ClosedVocabularyDialog::validateKeystroke()
{
    ui->keystroke->setText( QKeySequence(ui->keystroke->text()).toString() );
}
