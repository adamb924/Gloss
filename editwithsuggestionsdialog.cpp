#include "editwithsuggestionsdialog.h"
#include "ui_editwithsuggestionsdialog.h"

#include <QStringListModel>

EditWithSuggestionsDialog::EditWithSuggestionsDialog(const WritingSystem &ws, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditWithSuggestionsDialog)
{
    ui->setupUi(this);
    ui->lineEdit->setWritingSystem(ws);

    connect(ui->listView, SIGNAL(clicked(QModelIndex)), this, SLOT(copyValue(QModelIndex)) );

    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(copyValue(QModelIndex)) );
    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()) );

    mModel = 0;
}

EditWithSuggestionsDialog::~EditWithSuggestionsDialog()
{
    delete ui;
}

void EditWithSuggestionsDialog::setDefaultString( const QString & string )
{
    ui->lineEdit->setText( string );
}

void EditWithSuggestionsDialog::setSuggestions(const QStringList & strings )
{
    if( mModel != 0 )
    {
        delete mModel;
    }
    mModel = new QStringListModel( strings, this );
    ui->listView->setModel( mModel );
}

QString EditWithSuggestionsDialog::text() const
{
    return ui->lineEdit->text();
}

TextBit EditWithSuggestionsDialog::textBit() const
{
    return ui->lineEdit->textBit();
}

void EditWithSuggestionsDialog::copyValue(const QModelIndex & index)
{
    ui->lineEdit->setText( mModel->data(index, Qt::DisplayRole).toString() );
    ui->lineEdit->setFocus( Qt::OtherFocusReason );
}
