#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "project.h"
#include "databaseadapter.h"
#include "textdisplaywidget.h"
#include "newtextdialog.h"
#include "importflextextdialog.h"
#include "writingsystem.h"
#include "writingsystemsdialog.h"
#include "mergetranslationdialog.h"
#include "generictextinputdialog.h"
#include "searchquerymodel.h"
#include "xqueryinputdialog.h"
#include "databasequerydialog.h"
#include "replacedialog.h"
#include "singlephraseeditdialog.h"
#include "choosetextlinedialog.h"
#include "indexsearchmodel.h"
#include "lexiconedit.h"
#include "sqltabledialog.h"
#include "focus.h"

#include <QtGui>
#include <QtSql>
#include <QStringList>

// these may move
#include <QXmlQuery>
#include <QXmlResultItems>
#include "messagehandler.h"
#include "searchqueryview.h"
#include "mergeeafdialog.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    mProject = 0;

    ui->setupUi(this);

    setWindowTitle(tr("Gloss"));

    connect(ui->actionNew_Project, SIGNAL(triggered()), this, SLOT(newProject()));
    connect(ui->actionOpen_Project, SIGNAL(triggered()), this, SLOT(openProject()));
    connect(ui->actionSave_Project, SIGNAL(triggered()), this, SLOT(saveProject()));
    connect(ui->actionClose_Project, SIGNAL(triggered()), this, SLOT(closeProject()));
    connect(ui->actionClose_project_without_saving, SIGNAL(triggered()), this, SLOT(closeProjectWithoutSaving()));

    connect(ui->actionOpen_text, SIGNAL(triggered()), this, SLOT(openText()));
    connect(ui->actionAdd_text, SIGNAL(triggered()), this, SLOT(addBlankText()));
    connect(ui->actionImport_FlexText, SIGNAL(triggered()), this, SLOT(importFlexText()));
    connect(ui->actionImport_plain_text, SIGNAL(triggered()), this, SLOT(importPlainText()));
    connect(ui->actionWriting_systems, SIGNAL(triggered()), this, SLOT(writingSystems()) );

    connect(ui->actionDelete_text, SIGNAL(triggered()), this, SLOT(deleteText()) );
    connect(ui->actionMerge_translations_from_other_FlexText_file, SIGNAL(triggered()), this, SLOT(mergeTranslations()));
    connect(ui->actionBulk_merge_translations, SIGNAL(triggered()), this, SLOT(bulkMergeTranslations()));
    connect(ui->actionBulk_merge_EAF, SIGNAL(triggered()), this, SLOT(bulkMergeEaf()));
    connect(ui->actionMerge_EAF, SIGNAL(triggered()), this, SLOT(mergeEaf()));

    connect(ui->actionSearch_gloss_items, SIGNAL(triggered()), this, SLOT(searchGlossItems()));
    connect(ui->actionSubstring_search_gloss_items, SIGNAL(triggered()), this, SLOT(substringSearchGlossItems()));
    connect(ui->actionRaw_XQuery, SIGNAL(triggered()), this, SLOT(rawXQuery()));
    connect(ui->actionInterpreation_by_id, SIGNAL(triggered()), this, SLOT(searchForInterpretationById()));
    connect(ui->actionGloss_by_id, SIGNAL(triggered()), this, SLOT(searchForGlossById()));
    connect(ui->actionText_form_by_id, SIGNAL(triggered()), this, SLOT(searchForTextFormById()));

    connect(ui->actionRemove_unused_gloss_items, SIGNAL(triggered()), this, SLOT(removeUnusedGlossItems()));

    connect(ui->actionPerform_a_query, SIGNAL(triggered()), this, SLOT(sqlQueryDialog()));

    connect(ui->actionSet_text_XML_from_database, SIGNAL(triggered()), this, SLOT(setTextXmlFromDatabase()));

    connect(ui->actionGenerate_report_from_texts, SIGNAL(triggered()), this, SLOT(generateTextReport()));

    connect(ui->actionCounts_of_glosses, SIGNAL(triggered()), this, SLOT(countGlosses()));
    connect(ui->actionCounts_of_text_forms, SIGNAL(triggered()), this, SLOT(countTextForms()));
    connect(ui->actionSearch_and_replace, SIGNAL(triggered()), this, SLOT(searchAndReplace()));

    connect(ui->actionUnapproved_lines, SIGNAL(triggered()), this, SLOT(findUnapprovedLines()));
    connect(ui->actionApproved_lines, SIGNAL(triggered()), this, SLOT(findApprovedLines()));

    connect(ui->actionClose_and_save_open_texts, SIGNAL(triggered()), this, SLOT(closeOpenTexts()));

    connect(ui->actionOpen_text_line, SIGNAL(triggered()), this, SLOT(openTextLine()) );
    connect(ui->actionOpen_text_line_with_context, SIGNAL(triggered()), this, SLOT(openTextLineWithContext()));

    connect(ui->actionEdit_lexicon, SIGNAL(triggered()), this, SLOT(editLexicon()) );

    connect(ui->actionRebuild_index, SIGNAL(triggered()), this, SLOT(rebuildIndex()));

    connect( ui->actionLexical_entry_by_id, SIGNAL(triggered()), this, SLOT(searchForLexicalEntryById()) );
    connect( ui->actionAllomorph_by_id, SIGNAL(triggered()), this, SLOT(searchForAllomorphById()) );

    ui->actionSearch_files_instead_of_index->setCheckable(true);
    ui->actionSearch_files_instead_of_index->setChecked(false);

    setProjectActionsEnabled(false);

    addTableMenuItems();
}

MainWindow::~MainWindow()
{
    if( mProject != 0 )
        delete mProject;
    delete ui;
}

void MainWindow::addTableMenuItems()
{
    ui->menuGuts->addSeparator();

    QStringList tables;
    tables << "Interpretations" << "TextForms" << "Glosses" << "WritingSystems" << "Allomorph" << "LexicalEntry" << "LexicalEntryGloss" << "LexicalEntryCitationForm" << "LexicalEntryTags" << "MorphologicalAnalysisMembers" << "GrammaticalTags";
    foreach(QString table, tables)
    {
        QAction *action = new QAction(table,ui->menuGuts);
        action->setData(table);
        ui->menuGuts->addAction(action);
        QActionGroup *group = new QActionGroup(this);
        group->addAction(action);
        connect(group,SIGNAL(triggered(QAction*)),this,SLOT(sqlTableView(QAction*)));
    }
}

void MainWindow::newProject()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("New") );

    if( !filename.isNull() )
    {
        if( mProject != 0 )
            delete mProject;
        mProject = new Project(this);
        mProject->create(filename);

        setWindowTitle( tr("Gloss - %1").arg(filename) );
        setProjectActionsEnabled(true);
    }
}

void MainWindow::openProject()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open") );

    if( !filename.isNull() )
    {
        if( mProject != 0 )
            delete mProject;
        mProject = new Project(this);
        if( mProject->readFromFile(filename) )
        {
            setWindowTitle( tr("Gloss - %1").arg(filename) );
            setProjectActionsEnabled(true);
        }
        else
        {
            delete mProject;
            mProject = 0;
        }
    }
}

void MainWindow::saveProject()
{
    mProject->save();
}

void MainWindow::closeProject()
{
    if( mProject == 0 )
        return;
    mProject->save();
    projectClose();
}

void MainWindow::closeProjectWithoutSaving()
{
    projectClose();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if( mProject == 0 )
    {
        event->accept();
    }
    else if( maybeSave() )
    {
        projectClose();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::projectClose()
{
    QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
    while( windows.count() > 0 )
        ui->mdiArea->removeSubWindow( windows.takeAt(0) );

    mProject->removeTempDirectory();
    delete mProject;
    mProject = 0;
    setProjectActionsEnabled(false);
    setWindowTitle(tr("Gloss"));
}


bool MainWindow::maybeSave()
{
    while( ui->mdiArea->currentSubWindow() != 0 )
        ui->mdiArea->removeSubWindow(ui->mdiArea->currentSubWindow());

    QMessageBox::StandardButton result = QMessageBox::question(this,tr("Save changes?"),tr("Do you want to save your changes?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel , QMessageBox::Cancel );
    switch(result)
    {
    case QMessageBox::Yes:
        mProject->save();
        return true;
        break;
    case QMessageBox::No:
        return true;
        break;
    case QMessageBox::Cancel:
        return false;
        break;
    default:
        return false;
        break;
    }
}

void MainWindow::addBlankText()
{
    NewTextDialog dialog( mProject->dbAdapter()->writingSystems(), this);
    if( dialog.exec() == QDialog::Accepted )
    {
        Text *text = mProject->newText(dialog.name(), dialog.writingSystem());
        TextDisplayWidget *subWindow = new TextDisplayWidget(text, mProject, QList<Focus>(), this);
        ui->mdiArea->addSubWindow(subWindow);
        subWindow->show();
    }
}

WritingSystem MainWindow::selectWritingSystem(const QString & message, bool *ok)
{
    QList<WritingSystem> list = mProject->dbAdapter()->writingSystems();
    QStringList names;
    for(int i=0; i<list.count(); i++)
        names << list.at(i).name() + " (" + list.at(i).flexString() + ")";
    QString item = QInputDialog::getItem(this, tr("Select writing system"), message, names, 0, false, ok );
    if( names.contains(item) )
        return list.at( names.indexOf( item ) );
    else
        return WritingSystem();
}

void MainWindow::importPlainText()
{
    QFileDialog dialog( this, tr("Select file(s) to import"), QString(), "*.*" );
    dialog.setFileMode(QFileDialog::ExistingFiles);
    if( dialog.exec() == QDialog::Accepted )
    {
        bool ok;
        WritingSystem ws = selectWritingSystem(tr("Select the baseline writing system for these texts."), &ok);
        if( ok )
        {
            QStringList files = dialog.selectedFiles();
            bool openText = files.count() == 1;

            if( files.count() == 1 )
            {
                importPlainText( files.first() , ws , true );
            }
            else
            {
                QProgressDialog progress("Importing texts...", "Cancel", 0, files.count(), this);
                progress.setWindowModality(Qt::WindowModal);
                for(int i=0; i<files.count(); i++)
                {
                    progress.setValue(i);
                    if( QFile::exists(files.at(i)))
                        importPlainText( files.at(i) , ws , openText );
                    if( progress.wasCanceled() )
                        break;
                }
                progress.setValue(files.count());
            }
        }
    }
}

void MainWindow::importPlainText(const QString & filepath , const WritingSystem & ws, bool openText)
{
    QFile file(filepath);
    if( file.open(QFile::ReadOnly) )
    {
        QFileInfo info(filepath);
        QString name = info.baseName();

        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString content = stream.readAll();
        file.close();

        Text *text = mProject->newText(name, ws, content, openText);
        if( openText )
        {
            TextDisplayWidget *subWindow = new TextDisplayWidget(text, mProject, QList<Focus>(), this);
            ui->mdiArea->addSubWindow(subWindow);
            subWindow->show();
        }
    }
}

void MainWindow::importFlexText()
{
    ImportFlexTextDialog dialog(mProject->dbAdapter(),this);
    if( dialog.exec() == QDialog::Accepted )
    {
        if( QFile::exists(dialog.filename()) )
        {
            mProject->textFromFlexText(dialog.filename(),mProject->dbAdapter()->writingSystem(dialog.writingSystem()));
            Text *text = mProject->texts()->value( Text::textNameFromPath(dialog.filename()) );
            if(text != 0)
                openText(text->name());
        }
    }
}

void MainWindow::sqlTableView( QAction * action )
{
    QString name = action->data().toString();
    SqlTableDialog * dialog = new SqlTableDialog(name, mProject->dbAdapter());
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::setProjectActionsEnabled(bool enabled)
{
    ui->menuData->setEnabled(enabled);
    ui->menuGuts->setEnabled(enabled);
    ui->menuProject->setEnabled(enabled);
    ui->menuSearch->setEnabled(enabled);
    ui->menuReports->setEnabled(enabled);
    ui->actionAdd_text->setEnabled(enabled);
    ui->actionImport_FlexText->setEnabled(enabled);
    ui->actionSave_Project->setEnabled(enabled);
    ui->actionClose_Project->setEnabled(enabled);
    ui->actionWriting_systems->setEnabled(enabled);
    ui->actionClose_project_without_saving->setEnabled(enabled);
    ui->actionSearch_gloss_items->setEnabled(enabled);
    ui->actionEdit_lexicon->setEnabled(enabled);
}

void MainWindow::openText()
{
    if( mProject->textPaths()->count() == 0)
    {
        QMessageBox::information(this, tr("No texts"), tr("The project has no texts to open."));
        return;
    }
    bool ok;
    QString whichText = QInputDialog::getItem( this, tr("Select text"), tr("Select the text to open"), mProject->textNames(), 0, false, &ok );
    if( ok )
        openText(whichText);
}

TextDisplayWidget* MainWindow::openText(const QString & textName, const QList<Focus> & foci)
{
    Text *text;
    TextDisplayWidget *subWindow = 0;
    switch( mProject->openText(textName) )
    {
    case Project::Success:
        text = mProject->texts()->value(textName, 0);
        if( text != 0 )
        {
            subWindow = new TextDisplayWidget(text, mProject, foci, this);
            ui->mdiArea->addSubWindow(subWindow);
            subWindow->show();
        }
        break;
    case Project::FileNotFound:
        QMessageBox::critical(this, tr("Error opening file"), tr("Sorry, the text %1 could not be opened. The filename %2 could not be found.").arg(textName).arg(mProject->filepathFromName(textName)));
        break;
    case Project::XmlReadError:
        QMessageBox::critical(this, tr("Error opening file"), tr("Sorry, the text %1 could not be opened. There was a problem reading the XML.").arg(textName));
        break;
    }
    return subWindow;
}

void MainWindow::writingSystems()
{
    WritingSystemsDialog dialog(mProject->dbAdapter(), this);
    dialog.exec();
}

void MainWindow::deleteText()
{
    if( mProject->textPaths()->count() == 0)
    {
        QMessageBox::information(this, tr("No texts"), tr("The project has no texts to delete."));
        return;
    }
    bool ok;
    QString whichText = QInputDialog::getItem( this, tr("Select text"), tr("Select the text to delete"), mProject->textNames(), 0, false, &ok );
    if( ok )
    {
        if( QMessageBox::Yes == QMessageBox::warning(this, tr("Confirm"), tr("Are you sure you want to delete the text? (If you change your mind later, don't save this project when you close it.)')"), QMessageBox::Yes | QMessageBox::No , QMessageBox::No ) )
            mProject->deleteText(whichText);
    }
}

void MainWindow::mergeTranslations()
{
    MergeTranslationDialog dialog(mProject, this);
    if( dialog.exec() == QDialog::Accepted )
    {
        mProject->openText(dialog.text());
        Text *text = mProject->texts()->value(dialog.text());
        Text::MergeTranslationResult result = text->mergeTranslation( dialog.filename() , dialog.writingSystem() );
        mProject->closeText(text);
        switch(result)
        {
        case Text::Success:
            QMessageBox::information(0, tr("Success!"), tr("The merge into %1 has completed succesfully.").arg(dialog.text()));
            break;
        case Text::MergeStuckOldFileDeleted:
            QMessageBox::warning(0, tr("Error"), tr("The merge file is stuck with a temporary filename, but you can fix this yourself. The old flextext file has been deleted."));
            break;
        case Text::MergeStuckOldFileStillThere:
            QMessageBox::warning(0, tr("Error"), tr("The old flextext file could not be deleted, so the merge file is stuck with a temporary filename, but you can fix this yourself."));
            break;
        case Text::XslTranslationError:
            break;
        }
    }
}

void MainWindow::searchGlossItems()
{
    // Launch a dialog requesting input
    GenericTextInputDialog dialog(mProject->dbAdapter(), this);
    dialog.setWindowTitle(tr("Enter a search string"));
    if( dialog.exec() == QDialog::Accepted )
    {
        // Do the search of the texts
        QString query = QString("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                                "for $x in /document/interlinear-text/paragraphs/paragraph/phrases/phrase[descendant::item[@lang='%1' and text()='%2']] "
                                "let $line-number := string( $x/item[@type='segnum']/text() ) "
                                "let $count := string( count( $x/descendant::item[@lang='%1' and text()='%2'] ) ) "
                                "order by number($x/item[@type='segnum']/text()) "
                                "return   string-join( ($line-number, $count) , ',') ").arg(dialog.writingSystem().flexString()).arg(dialog.text());
        SearchQueryModel *model = new SearchQueryModel(query, mProject->textPaths(), this);
        createSearchResultDock(model, tr("Containing exact string '%1'").arg(dialog.text()) );
    }
}

void MainWindow::searchForInterpretationById(qlonglong id)
{
    QStandardItemModel *model;
    QList<Focus> foci;
    foci << Focus(Focus::Interpretation, id );

    if( ui->actionSearch_files_instead_of_index->isChecked() )
    {
        QString query = QString("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                                "for $x in /document/interlinear-text/paragraphs/paragraph/phrases/phrase[descendant::word[@abg:id='%1']] "
                                "let $line-number := string( $x/item[@type='segnum']/text() ) "
                                "let $count := string( count( $x/descendant::word[@abg:id='%1'] ) ) "
                                "order by number($x/item[@type='segnum']/text()) "
                                "return   string-join( ($line-number, $count) , ',') ").arg(id);
        model = new SearchQueryModel(query, mProject->textPaths(), this, foci);
    }
    else
    {
        if( !mProject->dbAdapter()->textIndicesExist() )
        {
            if( QMessageBox::Cancel == QMessageBox::information(this, tr("Patience..."), tr("This is your first search, so the text index needs to be built. It will be slow this one time, and after that it will be quite fast."), QMessageBox::Ok | QMessageBox::Cancel , QMessageBox::Ok ) )
                return;
            mProject->dbAdapter()->createTextIndices( mProject->textPaths() );
        }
        model = new IndexSearchModel( mProject->dbAdapter()->searchIndexForInterpretation( id ) , foci );
    }
    createSearchResultDock(model, tr("Interpretation ID: %1").arg(id));
}

void MainWindow::searchForTextFormById(qlonglong id)
{
    QStandardItemModel *model;
    QList<Focus> foci;
    foci << Focus(Focus::TextForm, id );
    if( ui->actionSearch_files_instead_of_index->isChecked() )
    {
        QString query = QString("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                                "for $x in /document/interlinear-text/paragraphs/paragraph/phrases/phrase[descendant::word/item[@abg:id='%1' and @type='txt']] "
                                "let $line-number := string( $x/item[@type='segnum']/text() ) "
                                "let $count := string( count( $x/descendant::word/item[@abg:id='%1' and @type='txt'] ) ) "
                                "order by number($x/item[@type='segnum']/text()) "
                                "return   string-join( ($line-number, $count) , ',') ").arg(id);
        model = new SearchQueryModel(query, mProject->textPaths(), this, foci);
    }
    else
    {
        if( !mProject->dbAdapter()->textIndicesExist() )
        {
            if( QMessageBox::Cancel == QMessageBox::information(this, tr("Patience..."), tr("This is your first search, so the text index needs to be built. It will be slow this one time, and after that it will be quite fast."), QMessageBox::Ok | QMessageBox::Cancel , QMessageBox::Ok ) )
                return;
            mProject->dbAdapter()->createTextIndices( mProject->textPaths() );
        }
        model = new IndexSearchModel( mProject->dbAdapter()->searchIndexForTextForm( id ), foci );
    }
    createSearchResultDock(model, tr("Text Form ID: %1").arg(id));
}

void MainWindow::searchForGlossById(qlonglong id)
{
    QStandardItemModel *model;
    QList<Focus> foci;
    foci << Focus(Focus::Gloss, id );

    if( ui->actionSearch_files_instead_of_index->isChecked() )
    {
        QString query = QString("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                                "for $x in /document/interlinear-text/paragraphs/paragraph/phrases/phrase[descendant::word/item[@abg:id='%1' and @type='gls']] "
                                "let $line-number := string( $x/item[@type='segnum']/text() ) "
                                "let $count := string( count( $x/descendant::word/item[@abg:id='%1' and @type='gls'] ) ) "
                                "order by number($x/item[@type='segnum']/text()) "
                                "return   string-join( ($line-number, $count) , ',') " ).arg(id);
        model = new SearchQueryModel(query, mProject->textPaths(), this, foci);
    }
    else
    {
        if( !mProject->dbAdapter()->textIndicesExist() )
        {
            if( QMessageBox::Cancel == QMessageBox::information(this, tr("Patience..."), tr("This is your first search, so the text index needs to be built. It will be slow this one time, and after that it will be quite fast."), QMessageBox::Ok | QMessageBox::Cancel , QMessageBox::Ok ) )
                return;
            mProject->dbAdapter()->createTextIndices( mProject->textPaths() );
        }
        model = new IndexSearchModel( mProject->dbAdapter()->searchIndexForGloss( id ), foci );
    }
    createSearchResultDock(model, tr("Gloss ID: %1").arg(id));
}

void MainWindow::searchForLexicalEntryById(qlonglong id)
{
    qDebug() << QDateTime::currentDateTime ().toString(Qt::ISODate);
    if( !mProject->dbAdapter()->textIndicesExist() )
    {
        if( QMessageBox::Cancel == QMessageBox::information(this, tr("Patience..."), tr("Searching for lexical entries requires the index to be buildt. This is your first search, so the text index needs to be built. It will be slow this one time, and after that it will be quite fast."), QMessageBox::Ok | QMessageBox::Cancel , QMessageBox::Ok ) )
            return;
        mProject->dbAdapter()->createTextIndices( mProject->textPaths() );
    }
    qDebug() << QDateTime::currentDateTime ().toString(Qt::ISODate);

    QList<Focus> foci;
    QSetIterator<qlonglong> iter( mProject->dbAdapter()->lexicalEntryTextFormIds( id ) );
    while( iter.hasNext() )
        foci << Focus( Focus::TextForm , iter.next() );

    qDebug() << QDateTime::currentDateTime ().toString(Qt::ISODate);
    QStandardItemModel *model = new IndexSearchModel( mProject->dbAdapter()->searchIndexForLexicalEntry( id ), foci );
    qDebug() << QDateTime::currentDateTime ().toString(Qt::ISODate);
    createSearchResultDock(model, tr("Lexical Entry ID: %1").arg(id));
    qDebug() << QDateTime::currentDateTime ().toString(Qt::ISODate);
}

void MainWindow::searchForAllomorphById(qlonglong id)
{
    if( !mProject->dbAdapter()->textIndicesExist() )
    {
        if( QMessageBox::Cancel == QMessageBox::information(this, tr("Patience..."), tr("Searching for lexical entries requires the index to be buildt. This is your first search, so the text index needs to be built. It will be slow this one time, and after that it will be quite fast."), QMessageBox::Ok | QMessageBox::Cancel , QMessageBox::Ok ) )
            return;
        mProject->dbAdapter()->createTextIndices( mProject->textPaths() );
    }

    QList<Focus> foci;
    QSetIterator<qlonglong> iter( mProject->dbAdapter()->allomorphTextFormIds( id ) );
    while( iter.hasNext() )
        foci << Focus( Focus::TextForm , iter.next() );

    QStandardItemModel *model = new IndexSearchModel( mProject->dbAdapter()->searchIndexForAllomorph( id ), foci );
    createSearchResultDock(model, tr("Allomorph ID: %1").arg(id));
}

void MainWindow::rebuildIndex()
{
    mProject->dbAdapter()->createTextIndices( mProject->textPaths() );
}

void MainWindow::searchForInterpretationById()
{
    bool ok;
    int id = QInputDialog::getInt ( this, tr("Search by interpretation ID"), tr("Interpretation ID"), 1, -2147483647, 2147483647, 1, &ok );
    if( ok )
        searchForInterpretationById(id);
}

void MainWindow::searchForTextFormById()
{
    bool ok;
    int id = QInputDialog::getInt ( this, tr("Search by text form ID"), tr("Text form ID"), 1, -2147483647, 2147483647, 1, &ok );
    if( ok )
        searchForTextFormById(id);
}

void MainWindow::searchForGlossById()
{
    bool ok;
    int id = QInputDialog::getInt ( this, tr("Search by gloss ID"), tr("Gloss ID"), 1, -2147483647, 2147483647, 1, &ok );
    if( ok )
        searchForGlossById(id);
}

void MainWindow::searchForLexicalEntryById()
{
    bool ok;
    int id = QInputDialog::getInt ( this, tr("Search by lexical entry ID"), tr("Lexical Entry ID"), 1, -2147483647, 2147483647, 1, &ok );
    if( ok )
        searchForLexicalEntryById(id);
}

void MainWindow::searchForAllomorphById()
{
    bool ok;
    int id = QInputDialog::getInt ( this, tr("Search by allomorph ID"), tr("Allomorph ID"), 1, -2147483647, 2147483647, 1, &ok );
    if( ok )
        searchForAllomorphById(id);
}

void MainWindow::substringSearchGlossItems()
{
    // Launch a dialog requesting input
    GenericTextInputDialog dialog(mProject->dbAdapter(), this);
    dialog.setWindowTitle(tr("Enter a search string"));
    if( dialog.exec() == QDialog::Accepted )
    {
        // Do the search of the texts
        QString query = QString("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                                "for $x in /document/interlinear-text/paragraphs/paragraph/phrases/phrase[descendant::item[@lang='%1' and contains( text(), '%2') ]] "
                                "let $line-number := string( $x/item[@type='segnum']/text() ) "
                                "let $count := string( count( $x/descendant::word[@lang='%1' and contains( text(), '%2') ] ) ) "
                                "order by number($x/item[@type='segnum']/text()) "
                                "return   string-join( ($line-number, $count) , ',') ").arg(dialog.writingSystem().flexString()).arg(dialog.text());
        SearchQueryModel *model = new SearchQueryModel(query, mProject->textPaths(), this);
        createSearchResultDock(model, tr("Items containing substring '%1'").arg(dialog.text()));
    }
}

void MainWindow::createSearchResultDock(QStandardItemModel * model, const QString & reminder)
{
    // Pop up a dockable window showing the results
    SearchQueryView *tree = new SearchQueryView(this);
    tree->setSortingEnabled(false);
    tree->setModel(model);
    tree->setHeaderHidden(true);
    tree->expandAll();

    connect( tree, SIGNAL(requestOpenText(QString,int,QList<Focus>)), this, SLOT(focusTextPosition(QString,int,QList<Focus>)));
    connect( tree, SIGNAL(requestPlaySound(QString,int)), this, SLOT(playSoundForLine(QString,int)) );
    connect( tree, SIGNAL(requestEditLine(QString,int,QList<Focus>)), this, SLOT(editLine(QString,int,QList<Focus>)) );
    connect( tree, SIGNAL(requestEditLineWithContext(QString,int,QList<Focus>)), this, SLOT(editLineWithContext(QString,int,QList<Focus>)) );

    QWidget *intermediateWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(intermediateWidget);
    layout->addWidget( new QLabel(reminder) );
    layout->addWidget(tree);
    intermediateWidget->setLayout(layout);

    QDockWidget *dock = new QDockWidget(tr("Search Results"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setWidget(intermediateWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
}

void MainWindow::focusTextPosition( const QString & textName , int lineNumber, const QList<Focus> & foci )
{
    QList<QMdiSubWindow*> windows = ui->mdiArea->subWindowList();
    QListIterator<QMdiSubWindow*> iter(windows);
    while(iter.hasNext())
    {
        QMdiSubWindow* w = iter.next();
        TextDisplayWidget* tdw = qobject_cast<TextDisplayWidget*>(w->widget());
        if( w->windowTitle() == textName && tdw != 0 )
        {
            tdw->focusGlossLine( lineNumber );
            return;
        }
    }
    // at this point the window must not exist
    TextDisplayWidget* tdw = openText(textName, foci);
    if( tdw != 0 && lineNumber != -1 )
        tdw->focusGlossLine(lineNumber);
}

void MainWindow::playSoundForLine( const QString & textName , int lineNumber )
{
    if( lineNumber == -1 )
        return;
    mProject->openText(textName);
    Text *text = mProject->texts()->value(textName, 0);
    if( text == 0)
        return;
    lineNumber--;
    text->playSoundForLine(lineNumber);
}

void MainWindow::editLine( const QString & textName , int lineNumber, const QList<Focus> & foci )
{
    if( lineNumber == -1 )
        return;
    mProject->openText(textName);
    Text *text = mProject->texts()->value(textName, 0);
    if( text == 0)
        return;

    lineNumber--; // make it 0-indexed instead

    if( lineNumber >= text->phrases()->count() )
    {
        QMessageBox::warning(this, tr("Error"), tr("%1 only has %2 phrases.").arg(text->name()).arg(text->phrases()->count()));
        return;
    }

    QList<int> lines;
    lines << lineNumber;

    SinglePhraseEditDialog *dialog = new SinglePhraseEditDialog(lines, mProject, text, foci);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::editLineWithContext( const QString & textName , int lineNumber, const QList<Focus> & foci )
{
    if( lineNumber == -1 )
        return;
    mProject->openText(textName);
    Text *text = mProject->texts()->value(textName, 0);
    if( text == 0)
        return;

    lineNumber--; // make it 0-indexed instead

    if( lineNumber >= text->phrases()->count() )
    {
        QMessageBox::warning(this, tr("Error"), tr("%1 only has %2 phrases.").arg(text->name()).arg(text->phrases()->count()));
        return;
    }

    QList<int> lines;
    if( lineNumber > 0 )
        lines << lineNumber-1;
    lines << lineNumber;
    if( lineNumber < text->phrases()->count()-1 )
        lines << lineNumber+1;

    SinglePhraseEditDialog *dialog = new SinglePhraseEditDialog(lines, mProject, text, foci);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::rawXQuery()
{
    // Launch a dialog requesting input
    XQueryInputDialog dialog(this);
    dialog.setWindowTitle(tr("Perform a raw XQuery on all texts"));
    if( dialog.exec() == QDialog::Accepted )
    {
        SearchQueryModel *model = new SearchQueryModel(dialog.query(), mProject->textPaths(), this);
        createSearchResultDock(model, tr("Raw XQuery"));
    }
}

void MainWindow::removeUnusedGlossItems()
{
    QString report = mProject->doDatabaseCleanup();
    QMessageBox::information(this, tr("Process complete"), report );
}

void MainWindow::sqlQueryDialog()
{
    DatabaseQueryDialog *dialog = new DatabaseQueryDialog(mProject->dbAdapter()->dbFilename(), 0);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::bulkMergeTranslations()
{
    bool ok;
    WritingSystem ws = selectWritingSystem(tr("Select the translations' writing system."), &ok);

    QString directory = QFileDialog::getExistingDirectory(this, tr("Choose directory with translations") );
    if( !directory.isEmpty() )
    {
        QDir dir(directory);
        dir.setFilter(QDir::Files);
        dir.setNameFilters(QStringList("*.flextext"));
        dir.setSorting(QDir::Name);
        QStringList translationFiles = dir.entryList(QDir::Files,QDir::Name);

        int total = translationFiles.count();
        int successes = 0;
        int failures = 0;
        int nonmatches = 0;
        QStringList failureNames;
        QStringList unmatchedNames;

        QStringList flextextNames = mProject->textNames();

        QProgressDialog progress(tr("Merging translations..."), "Cancel", 0, translationFiles.count(), 0);
        progress.setWindowModality(Qt::WindowModal);

        for(int i=0; i<translationFiles.count(); i++)
        {
            progress.setValue(i);

            QString filePath = dir.absoluteFilePath(translationFiles.at(i));
            QString textName = translationFiles.at(i);
            textName.replace(QRegExp("\\.flextext$"),"");

            if( flextextNames.contains(textName) )
            {
                mProject->openText(textName);
                Text *text = mProject->texts()->value(textName);
                if( text->mergeTranslation( filePath , ws ) == Text::Success )
                {
                    successes++;
                }
                else
                {
                    failures++;
                    failureNames << textName;
                }
                mProject->closeText(text);
            }
            else
            {
                nonmatches++;
                unmatchedNames << textName;
            }

            if( progress.wasCanceled() )
                break;
        }
        progress.setValue(translationFiles.count());

        QMessageBox::information(this, tr("Result"), tr("%1 available translations, %2 that don't match (%3), %4 success(es), %5 failure(s) (%6). If there were failures you might get more information by merging them individually.").arg(total).arg(nonmatches).arg(unmatchedNames.join(", ")).arg(successes).arg(failures).arg(failureNames.join(", ")));
    }
}

void MainWindow::mergeEaf()
{
    MergeEafDialog dialog(mProject, this);
    if( dialog.exec() == QDialog::Accepted )
    {
        mProject->openText(dialog.text());
        Text *text = mProject->texts()->value(dialog.text());
        Text::MergeEafResult result = text->mergeEaf( dialog.filename() );
        mProject->saveAndCloseText(text);
        switch(result)
        {
        case Text::Success:
            QMessageBox::information(0, tr("Success!"), tr("The merge into %1 has completed succesfully.").arg(dialog.text()));
            break;
        case Text::MergeEafWrongNumberOfAnnotations:
            QMessageBox::information(0, tr("Failure!"), tr("The merge into %1 has failed because the number of annotations is wrong.").arg(dialog.text()));
        default:
            QMessageBox::information(0, tr("Failure!"), tr("The merge into %1 has failed.").arg(dialog.text()));
            break;
        }
    }
}

void MainWindow::bulkMergeEaf()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr("Choose directory with Elan files") );
    if( !directory.isEmpty() )
    {
        QDir dir(directory);
        dir.setFilter(QDir::Files);
        dir.setNameFilters(QStringList("*.eaf"));
        dir.setSorting(QDir::Name);
        QStringList eafFiles = dir.entryList(QDir::Files,QDir::Name);

        int total = eafFiles.count();
        int successes = 0;
        int failures = 0;
        int nonmatches = 0;
        QStringList failureNames;
        QStringList unmatchedNames;

        QStringList flextextNames = mProject->textNames();

        QProgressDialog progress(tr("Merging EAF..."), "Cancel", 0, eafFiles.count(), 0);
        progress.setWindowModality(Qt::WindowModal);

        for(int i=0; i<eafFiles.count(); i++)
        {
            progress.setValue(i);

            QString filePath = dir.absoluteFilePath(eafFiles.at(i));
            QString textName = eafFiles.at(i);
            textName.replace(QRegExp("\\.eaf"),"");

            if( flextextNames.contains(textName) )
            {
                mProject->openText(textName);
                Text *text = mProject->texts()->value(textName);
                if( text->mergeEaf( filePath ) == Text::MergeEafSuccess )
                {
                    successes++;
                }
                else
                {
                    failures++;
                    failureNames << textName;
                }
                mProject->saveAndCloseText(text);
            }
            else
            {
                nonmatches++;
                unmatchedNames << textName;
            }

            if( progress.wasCanceled() )
                break;
        }
        progress.setValue(eafFiles.count());

        QMessageBox::information(this, tr("Result"), tr("%1 available EAF files, %2 that don't match (%3), %4 success(es), %5 failure(s) (%6). If there were failures you might get more information by merging them individually.").arg(total).arg(nonmatches).arg(unmatchedNames.join(", ")).arg(successes).arg(failures).arg(failureNames.join(", ")));
    }
}

void MainWindow::setTextXmlFromDatabase()
{
    mProject->setTextXmlFromDatabase();
    QMessageBox::information(this, tr("Done!"), tr("The text files have been updated."));
}

void MainWindow::generateTextReport()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save report"), QString(), tr("Comma separated values (*.csv)") );
    if( !filename.isNull() )
    {
        if( mProject->interpretationUsageReport(filename) )
        {
            if( QMessageBox::Yes == QMessageBox::question(this, tr("Gloss"), tr("The report is finished; would you like to open it?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes ) )
                QDesktopServices::openUrl(QUrl(filename, QUrl::TolerantMode));
        }
        else
        {
            QMessageBox::warning(this, tr("Gloss"), tr("The report could not be generated, sorry.") );
        }
    }
}

void MainWindow::countTextForms()
{
    createCountReport("txt");
}

void MainWindow::countGlosses()
{
    createCountReport("gls");
}

void MainWindow::createCountReport(const QString & typeString)
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save report"), QString(), tr("Comma separated values (*.csv)") );
    if( !filename.isNull() )
    {
        if( mProject->countItemsInTexts(filename, typeString) )
        {
            if( QMessageBox::Yes == QMessageBox::question(this, tr("Gloss"), tr("The report is finished; would you like to open it?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes ) )
                QDesktopServices::openUrl(QUrl(filename, QUrl::TolerantMode));
        }
        else
        {
            QMessageBox::warning(this, tr("Gloss"), tr("The report could not be generated, sorry.") );
        }
    }
}

void MainWindow::searchAndReplace()
{
    ReplaceDialog dialog(mProject->dbAdapter(), this);
    if( dialog.exec() == QDialog::Accepted )
    {
        QHash<QString,QString> parameters;
        parameters.insert( "transformation-schema" , dialog.xmlFilePath() );
        mProject->applyXslTransformationToTexts( QDir::current().absoluteFilePath("search-replace.xsl"), parameters );
        QMessageBox::information(this, tr("Gloss"), tr("The search-and-replace operation has been completed.") );
    }
}

void MainWindow::findApprovedLines()
{
    QString query = QString("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                            "for $x in /document/interlinear-text/paragraphs/paragraph/phrases/phrase[count(words/word/@abg:approval-status='false')=0] "
                            "order by number($x/item[@type='segnum']/text()) "
                            "return string( $x/item[@type='segnum']/text() )");
    SearchQueryModel *model = new SearchQueryModel(query, mProject->textPaths(), this);
    createSearchResultDock(model, tr("Approved lines") );
}

void MainWindow::findUnapprovedLines()
{
    QString query = QString("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                            "for $x in /document/interlinear-text/paragraphs/paragraph/phrases/phrase[exists(words/word/@abg:approval-status='false')] "
                            "order by number($x/item[@type='segnum']/text()) "
                            "return string( $x/item[@type='segnum']/text() )");
    SearchQueryModel *model = new SearchQueryModel(query, mProject->textPaths(), this);
    createSearchResultDock(model, tr("Unapproved lines") );
}

void MainWindow::closeOpenTexts()
{
    mProject->saveOpenTexts();
    mProject->closeOpenTexts( textsWithOpenWindows() );
}

void MainWindow::openTextLine()
{
    ChooseTextLineDialog dialog(mProject->textNames(), this);
    if( dialog.exec() == QDialog::Accepted )
        editLine( dialog.textName() , dialog.lineNumber(), QList<Focus>() );
}

void MainWindow::openTextLineWithContext()
{
    ChooseTextLineDialog dialog(mProject->textNames(), this);
    if( dialog.exec() == QDialog::Accepted )
        editLineWithContext( dialog.textName() , dialog.lineNumber(), QList<Focus>() );
}

QStringList MainWindow::textsWithOpenWindows()
{
    QStringList textNames;
    foreach( QMdiSubWindow * window , ui->mdiArea->subWindowList() )
        textNames << window->windowTitle();
    return textNames;
}

void MainWindow::editLexicon()
{
    LexiconEdit *edit = new LexiconEdit( mProject->dbAdapter(), this );
    connect( edit, SIGNAL(textFormIdSearch(qlonglong)), this, SLOT(searchForTextFormById(qlonglong)) );
    edit->show();
}
