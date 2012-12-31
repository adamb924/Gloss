#include "genericlexicalentryform.h"
#include "ui_genericlexicalentryform.h"

GenericLexicalEntryForm::GenericLexicalEntryForm(const WritingSystem & ws, const QHash<qlonglong,TextBit> & candidates, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GenericLexicalEntryForm)
{
    ui->setupUi(this);

    // TODO maybe we don't need to store these
    mWritingSystem = ws;
    mCandidates = candidates;

    ui->lineEdit->setWritingSystem(ws);
    ui->languageLabel->setText(ws.name());

    ui->otherCombo->addItem( tr("<New value>") , -1 );
    QHashIterator<qlonglong, TextBit> iter(candidates);
    while( iter.hasNext() )
    {
        iter.next();
        ui->otherCombo->addItem( iter.value().text() , iter.key() );
    }
}

GenericLexicalEntryForm::~GenericLexicalEntryForm()
{
    delete ui;
}

qlonglong GenericLexicalEntryForm::id() const
{
    return ui->otherCombo->itemData( ui->otherCombo->currentIndex() ).toLongLong();
}

QString GenericLexicalEntryForm::text() const
{
    if( id() == -1 )
        return ui->lineEdit->text();
    else
        return ui->otherCombo->currentText();
}
