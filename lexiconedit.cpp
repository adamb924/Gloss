#include "lexiconedit.h"
#include "ui_lexiconedit.h"

#include "lexiconstandardmodel.h"
#include "allomorphmodel.h"
#include "morphologicalanalysismodel.h"
#include "mainwindow.h"
#include "tagmodel.h"

#include <QSortFilterProxyModel>

LexiconEdit::LexiconEdit(const DatabaseAdapter * dbAdapter, const MainWindow * mainWnd, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LexiconEdit)
{
    ui->setupUi(this);

    LexiconStandardModel *lexiconModel = new LexiconStandardModel( dbAdapter );
    QSortFilterProxyModel * lexiconProxyModel = new QSortFilterProxyModel(this);
    lexiconProxyModel->setSourceModel( lexiconModel );
    lexiconProxyModel->setFilterKeyColumn(-1);
    ui->lexiconTable->setModel( lexiconProxyModel );

    connect( ui->filterEdit, SIGNAL(textChanged(QString)), lexiconProxyModel, SLOT(setFilterRegExp(QString)) );

    ui->lexiconTable->setSortingEnabled(true);
    ui->allomorphTable->setSortingEnabled(true);
    ui->analysisTable->setSortingEnabled(true);

    AllomorphModel *allomorphModel = new AllomorphModel( dbAdapter );
    QSortFilterProxyModel * allomorphProxyModel = new QSortFilterProxyModel(this);
    allomorphProxyModel->setSourceModel( allomorphModel );
    allomorphProxyModel->setFilterKeyColumn(-1);
    ui->allomorphTable->setModel( allomorphProxyModel );

    connect( ui->lexiconTable, SIGNAL(lexicalEntrySelected(qlonglong)), allomorphModel, SLOT(setLexicalEntry(qlonglong)) );

    MorphologicalAnalysisModel *analysisModel = new MorphologicalAnalysisModel( dbAdapter );
    QSortFilterProxyModel * analysisProxyModel = new QSortFilterProxyModel(this);
    analysisProxyModel->setSourceModel( analysisModel );
    analysisProxyModel->setFilterKeyColumn(-1);
    ui->analysisTable->setModel( analysisProxyModel );

    TagModel *allTags = new TagModel( dbAdapter );
    allTags->setLexicalEntry(-1);
    ui->allTags->setModel(allTags);
    ui->allTags->setDragEnabled(true);

    TagModel *lexicalEntryTags = new TagModel( dbAdapter );
    ui->lexicalEntryTags->setModel(lexicalEntryTags);
    ui->lexicalEntryTags->setDragEnabled(true);

    connect( ui->lexiconTable, SIGNAL(lexicalEntrySelected(qlonglong)), lexicalEntryTags, SLOT(setLexicalEntry(qlonglong)));

    connect( ui->lexiconTable, SIGNAL(lexicalEntrySelected(qlonglong)), analysisModel, SLOT(setLexicalEntry(qlonglong)));
    connect( ui->analysisTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(analysisDoubleClicked(QModelIndex)) );
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
