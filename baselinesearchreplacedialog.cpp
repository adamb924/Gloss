#include "baselinesearchreplacedialog.h"
#include "ui_baselinesearchreplacedialog.h"

#include "databaseadapter.h"
#include "lingedit.h"

BaselineSearchReplaceDialog::BaselineSearchReplaceDialog(const DatabaseAdapter *dbAdapter, const WritingSystem & defaultWs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BaselineSearchReplaceDialog)
{
    ui->setupUi(this);

    mDbAdapter = dbAdapter;

    connect( ui->writingSystemCombo, SIGNAL(writingSystemSelected(WritingSystem)), ui->searchEdit, SLOT(setWritingSystem(WritingSystem)) );
    connect( ui->writingSystemCombo, SIGNAL(writingSystemSelected(WritingSystem)), ui->replaceEdit, SLOT(setWritingSystem(WritingSystem)) );

    connect( ui->replaceEdit, SIGNAL(textChanged(QString)), this, SLOT(offerReplaceFeedback()) );
    connect( ui->searchEdit, SIGNAL(textChanged(QString)), this, SLOT(offerSearchFeedback()) );

    ui->writingSystemCombo->setWritingSystems( mDbAdapter->writingSystems() );
    ui->writingSystemCombo->setCurrentWritingSystem( defaultWs );
}

BaselineSearchReplaceDialog::~BaselineSearchReplaceDialog()
{
    delete ui;
}

void BaselineSearchReplaceDialog::offerSearchFeedback()
{
    QList<qlonglong> candidates = mDbAdapter->candidateInterpretations( ui->searchEdit->textBit() );
    if( candidates.count() == 0 )
    {
        ui->searchLabel->setText(tr("No matching text forms"));
    }
    else if( candidates.count() == 1 )
    {
        ui->searchLabel->setText(tr("One matching text form"));
    }
    else
    {
        ui->replaceLabel->setText(tr("Multiple matching text forms"));
    }
}

void BaselineSearchReplaceDialog::offerReplaceFeedback()
{
    QList<qlonglong> candidates = mDbAdapter->candidateInterpretations( ui->replaceEdit->textBit() );
    if( candidates.count() == 0 )
    {
        ui->replaceLabel->setText(tr("There are currently no interpretations for this form"));
    }
    else if( candidates.count() == 1 )
    {
        QHash<qlonglong,QString> summaries = mDbAdapter->candidateInterpretationWithSummaries(ui->replaceEdit->textBit());
        ui->replaceLabel->setText(tr("There is one possible interpretation for this form:<br/>%1").arg( summaries.value( candidates.first() ) ));
    }
    else
    {
        QHash<qlonglong,QString> summaries = mDbAdapter->candidateInterpretationWithSummaries(ui->replaceEdit->textBit());
        ui->replaceLabel->setText(tr("There are multiple interpretations for this replacement. The default will be:<br>%1").arg( summaries.value( candidates.first() ) ));
    }
}

TextBit BaselineSearchReplaceDialog::searchString() const
{
    return ui->searchEdit->textBit();
}

TextBit BaselineSearchReplaceDialog::replaceString() const
{
    return ui->replaceEdit->textBit();
}

