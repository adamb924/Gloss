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
#include "project.h"

#include <QSortFilterProxyModel>

LexiconEdit::LexiconEdit(const Project * project, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LexiconEdit),
    mProject(project)
{
    ui->setupUi(this);

    mDbAdapter = mProject->dbAdapter();

    AllTagsModel *allTags = new AllTagsModel( mDbAdapter , this );
    ui->allTags->setModel(allTags);
    ui->allTags->setDragEnabled(true);

    TagModel *lexicalEntryTags = new TagModel( mDbAdapter , this );
    ui->lexicalEntryTags->setModel(lexicalEntryTags);
    ui->lexicalEntryTags->setDragDropMode(QAbstractItemView::DropOnly);

    mLexiconModel = new LexiconModel( allTags , mProject , this );
    mLexiconProxyModel = new QSortFilterProxyModel(this);
    mLexiconProxyModel->setSourceModel( mLexiconModel );
    mLexiconProxyModel->setFilterKeyColumn(-1);
    ui->lexiconTable->setModel( mLexiconProxyModel );
    ui->lexiconTable->setColumnHidden(1,true);
    ui->lexiconTable->setColumnHidden(3,true);
    ui->lexiconTable->setColumnHidden(5,true);
    ui->lexiconTable->setColumnHidden(7,true);

    connect( ui->filterEdit, SIGNAL(textChanged(QString)), mLexiconProxyModel, SLOT(setFilterRegExp(QString)) );

    ui->lexiconTable->setSortingEnabled(true);
    ui->allomorphTable->setSortingEnabled(true);
    ui->analysisTable->setSortingEnabled(true);

    AllomorphModel *allomorphModel = new AllomorphModel( mDbAdapter , this );
    QSortFilterProxyModel * allomorphProxyModel = new QSortFilterProxyModel(this);
    allomorphProxyModel->setSourceModel( allomorphModel );
    allomorphProxyModel->setFilterKeyColumn(-1);
    ui->allomorphTable->setModel( allomorphProxyModel );

    connect( ui->lexiconTable, SIGNAL(lexicalEntrySelected(qlonglong)), allomorphModel, SLOT(setLexicalEntry(qlonglong)) );
    connect( ui->lexiconTable, SIGNAL(lexicalEntrySelected(qlonglong)), ui->allomorphTable, SLOT(resizeColumnsToContents()) );

    MorphologicalAnalysisModel *analysisModel = new MorphologicalAnalysisModel( mDbAdapter , this );

    connect( ui->glossWSCombo, SIGNAL(writingSystemSelected(WritingSystem)), analysisModel, SLOT(setGlossWritingSystem(WritingSystem)) );
    connect( ui->textWSCombo, SIGNAL(writingSystemSelected(WritingSystem)), analysisModel, SLOT(setTextFormWritingSystem(WritingSystem)) );

    ui->glossWSCombo->setWritingSystems( mDbAdapter->writingSystems() );
    ui->textWSCombo->setWritingSystems( mDbAdapter->writingSystems() );

    ui->glossWSCombo->setCurrentWritingSystem( mProject->defaultGlossLanguage() );
    ui->textWSCombo->setCurrentWritingSystem( mProject->defaultTextFormLanguage() );

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

    connect( ui->filterSortColumnOnlyCheckbox, SIGNAL(toggled(bool)), this, SLOT(setFilterAppliesToSortColumnOnly()));
    setFilterAppliesToSortColumnOnly( );
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

void LexiconEdit::setFilterAppliesToSortColumnOnly( )
{
    if( ui->filterSortColumnOnlyCheckbox )
        mLexiconProxyModel->setFilterKeyColumn( mLexiconProxyModel->sortColumn() );
    else
        mLexiconProxyModel->setFilterKeyColumn(-1);
}
