#include "lexiconedit.h"
#include "ui_lexiconedit.h"

#include "lexiconmodel.h"
#include "allomorphmodel.h"
#include "morphologicalanalysismodel.h"
#include "mainwindow.h"
#include "tagmodel.h"
#include "alltagsmodel.h"
#include "writingsystemcombo.h"
#include "databaseadapter.h"
#include "editwithsuggestionsdialog.h"

#include <QSortFilterProxyModel>

LexiconEdit::LexiconEdit(const DatabaseAdapter * dbAdapter, const MainWindow * mainWnd, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LexiconEdit)
{
    ui->setupUi(this);

    mDbAdapter = dbAdapter;

    AllTagsModel *allTags = new AllTagsModel( dbAdapter , this );
    ui->allTags->setModel(allTags);
    ui->allTags->setDragEnabled(true);

    TagModel *lexicalEntryTags = new TagModel( dbAdapter , this );
    ui->lexicalEntryTags->setModel(lexicalEntryTags);
    ui->lexicalEntryTags->setDragDropMode(QAbstractItemView::DropOnly);

    mLexiconModel = new LexiconModel( allTags , dbAdapter , this );
    QSortFilterProxyModel * lexiconProxyModel = new QSortFilterProxyModel(this);
    lexiconProxyModel->setSourceModel( mLexiconModel );
    lexiconProxyModel->setFilterKeyColumn(-1);
    ui->lexiconTable->setModel( lexiconProxyModel );
    ui->lexiconTable->setColumnHidden(1,true);
    ui->lexiconTable->setColumnHidden(3,true);
    ui->lexiconTable->setColumnHidden(5,true);
    ui->lexiconTable->setColumnHidden(7,true);

    connect( ui->filterEdit, SIGNAL(textChanged(QString)), lexiconProxyModel, SLOT(setFilterRegExp(QString)) );

    ui->lexiconTable->setSortingEnabled(true);
    ui->allomorphTable->setSortingEnabled(true);
    ui->analysisTable->setSortingEnabled(true);

    AllomorphModel *allomorphModel = new AllomorphModel( dbAdapter , this );
    QSortFilterProxyModel * allomorphProxyModel = new QSortFilterProxyModel(this);
    allomorphProxyModel->setSourceModel( allomorphModel );
    allomorphProxyModel->setFilterKeyColumn(-1);
    ui->allomorphTable->setModel( allomorphProxyModel );

    connect( ui->lexiconTable, SIGNAL(lexicalEntrySelected(qlonglong)), allomorphModel, SLOT(setLexicalEntry(qlonglong)) );
    connect( ui->lexiconTable, SIGNAL(lexicalEntrySelected(qlonglong)), ui->allomorphTable, SLOT(resizeColumnsToContents()) );

    MorphologicalAnalysisModel *analysisModel = new MorphologicalAnalysisModel( dbAdapter , this );

    connect( ui->glossWSCombo, SIGNAL(writingSystemSelected(WritingSystem)), analysisModel, SLOT(setGlossWritingSystem(WritingSystem)) );
    connect( ui->textWSCombo, SIGNAL(writingSystemSelected(WritingSystem)), analysisModel, SLOT(setTextFormWritingSystem(WritingSystem)) );

    ui->glossWSCombo->setWritingSystems( dbAdapter->writingSystems() );
    ui->textWSCombo->setWritingSystems( dbAdapter->writingSystems() );

    ui->glossWSCombo->setCurrentWritingSystem( dbAdapter->defaultGlossLanguage() );
    ui->textWSCombo->setCurrentWritingSystem( dbAdapter->defaultTextFormLanguage() );

    QSortFilterProxyModel * analysisProxyModel = new QSortFilterProxyModel(this);
    analysisProxyModel->setSourceModel( analysisModel );
    analysisProxyModel->setFilterKeyColumn(-1);
    ui->analysisTable->setModel( analysisProxyModel );

    connect( ui->lexiconTable, SIGNAL(lexicalEntrySelected(qlonglong)), lexicalEntryTags, SLOT(setLexicalEntry(qlonglong)));
    connect( ui->lexiconTable, SIGNAL(requestEditForm(QModelIndex)), this, SLOT(editForm(QModelIndex)) );

    connect( ui->lexiconTable, SIGNAL(lexicalEntrySelected(qlonglong)), analysisModel, SLOT(setLexicalEntry(qlonglong)));
    connect( ui->lexiconTable, SIGNAL(lexicalEntrySelected(qlonglong)), ui->analysisTable, SLOT(resizeColumnsToContents()) );

    connect( ui->analysisTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(analysisDoubleClicked(QModelIndex)) );

    connect( ui->refreshButton, SIGNAL(clicked()), mLexiconModel, SLOT(refreshQuery()) );
}

LexiconEdit::~LexiconEdit()
{
    delete ui;
}

void LexiconEdit::analysisDoubleClicked( const QModelIndex & index )
{
    bool ok;
    qlonglong textFormIndex = index.data().toString().toLongLong(&ok);
    if( ok )
        emit textFormIdSearch( textFormIndex );
}

void LexiconEdit::editForm( const QModelIndex & index )
{
    qlonglong lexicalEntryId = index.data(Qt::UserRole).toLongLong();
    int column = index.column();

    if ( mLexiconModel->typeFromColumn(column) == LexiconModel::CitationForm )
    {
        WritingSystem ws = mLexiconModel->writingSystemFromColumn(column);
        if( ws.isValid() )
        {
            EditWithSuggestionsDialog dialog( ws, this );
            dialog.setWindowTitle(tr("Edit the %1 citation form").arg(ws.name()));
            dialog.setDefaultString( mDbAdapter->guessLexicalEntryCitationForm(lexicalEntryId, ws) );
            dialog.setSuggestions( mDbAdapter->suggestLexicalEntryCitationForms(lexicalEntryId, ws) );
            if( dialog.exec() == QDialog::Accepted )
            {
                mDbAdapter->updateLexicalEntryCitationForm(lexicalEntryId, dialog.textBit() );
                mLexiconModel->refreshQuery();
            }
        }
    }
    else if ( mLexiconModel->typeFromColumn(column) == LexiconModel::Gloss )
    {
        WritingSystem ws = mLexiconModel->writingSystemFromColumn(column);
        if( ws.isValid() )
        {
            EditWithSuggestionsDialog dialog( ws, this );
            dialog.setWindowTitle(tr("Edit the %1 gloss").arg(ws.name()));
            dialog.setDefaultString( mDbAdapter->guessLexicalEntryGloss(lexicalEntryId, ws) );
            dialog.setSuggestions( mDbAdapter->suggestLexicalEntryGlosses( lexicalEntryId, ws) );
            if( dialog.exec() == QDialog::Accepted )
            {
                mDbAdapter->updateLexicalEntryGloss(lexicalEntryId, dialog.textBit() );
                mLexiconModel->refreshQuery();
            }
        }
    }
}
