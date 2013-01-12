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

#include <QtGui>
#include <QtSql>
#include <QStringList>

// these may move
#include <QXmlQuery>
#include <QXmlResultItems>
#include "messagehandler.h"

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
    tables << "Interpretations" << "TextForms" << "Glosses" << "WritingSystems" << "Allomorph" << "LexicalEntry" << "LexicalEntryGloss" << "LexicalEntryCitationForm" << "MorphologicalAnalysisMembers";
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
        mProject = new Project();
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
        mProject = new Project();
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
        TextDisplayWidget *subWindow = new TextDisplayWidget(text, mProject, this);
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
            TextDisplayWidget *subWindow = new TextDisplayWidget(text, mProject, this);
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
            Text *text = mProject->textFromFlexText(dialog.filename(),mProject->dbAdapter()->writingSystem(dialog.writingSystem()));
            if(text != 0)
                openText(text->name());
        }
        else
        {
            QMessageBox::critical(this,tr("Error reading file"),tr("The file %1 could not be opened.").arg(dialog.filename()));
        }
    }
}

void MainWindow::sqlTableView( QAction * action )
{
    QString name = action->data().toString();
    QSqlTableModel *model = new QSqlTableModel(this,QSqlDatabase::database(mProject->dbAdapter()->dbFilename()));
    model->setTable(name);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    QTableView *view = new QTableView;
    view->setModel(model);
    view->setSortingEnabled(true);
    view->setWindowTitle(name);
    view->show();
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

TextDisplayWidget* MainWindow::openText(const QString & textName)
{
    if( ! mProject->openText(textName) )
    {
        QMessageBox::critical(this, tr("Error opening file"), tr("Sorry, the text %1 could not be opened.").arg(textName));
        return 0;
    }
    Text *text = mProject->texts()->value(textName, 0);
    if( text == 0 )
    {
        QMessageBox::critical(this, tr("Error opening file"), tr("Sorry, the text %1 could not be opened.").arg(textName));
        return 0;
    }
    else
    {
        TextDisplayWidget *subWindow = new TextDisplayWidget(text, mProject, this);
        ui->mdiArea->addSubWindow(subWindow);
        subWindow->show();
        return subWindow;
    }
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
        QString query = QString("declare namespace abg = \"http://www.adambaker.org/gloss.php\"; for $x in /document/interlinear-text/paragraphs/paragraph/phrases/phrase[descendant::item[@lang='%1' and text()='%2']] return string( $x/item[@type='segnum']/text() )").arg(dialog.writingSystem().flexString()).arg(dialog.text());
        createSearchResultDock(query);
    }
}

void MainWindow::searchForInterpretationById()
{
    bool ok;
    int id = QInputDialog::getInt ( this, tr("Search by interpretation ID"), tr("Interpretation ID"), 1, 1, 2147483647, 1, &ok );
    if( ok )
    {
        QString query = QString("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                                "for $x in /document/interlinear-text/paragraphs/paragraph/phrases/phrase[descendant::word[@abg:id='%1']] "
                                "return string( $x/item[@type='segnum']/text() )").arg(id);
        createSearchResultDock(query);
    }
}

void MainWindow::searchForTextFormById()
{
    bool ok;
    int id = QInputDialog::getInt ( this, tr("Search by text form ID"), tr("Text form ID"), 1, 1, 2147483647, 1, &ok );
    if( ok )
    {
        QString query = QString("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                                "for $x in /document/interlinear-text/paragraphs/paragraph/phrases/phrase[descendant::word/item[@abg:id='%1' and @type='txt']] "
                                "return string( $x/item[@type='segnum']/text() )").arg(id);
        createSearchResultDock(query);
    }
}

void MainWindow::searchForGlossById()
{
    bool ok;
    int id = QInputDialog::getInt ( this, tr("Search by gloss ID"), tr("Gloss ID"), 1, 1, 2147483647, 1, &ok );
    if( ok )
    {
        QString query = QString("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                                "for $x in /document/interlinear-text/paragraphs/paragraph/phrases/phrase[descendant::word/item[@abg:id='%1' and @type='gls']] "
                                "return string( $x/item[@type='segnum']/text() )").arg(id);
        createSearchResultDock(query);
    }
}

void MainWindow::substringSearchGlossItems()
{
    // Launch a dialog requesting input
    GenericTextInputDialog dialog(mProject->dbAdapter(), this);
    dialog.setWindowTitle(tr("Enter a search string"));
    if( dialog.exec() == QDialog::Accepted )
    {
        // Do the search of the texts
        QString query = QString("declare namespace abg = \"http://www.adambaker.org/gloss.php\"; for $x in /document/interlinear-text/paragraphs/paragraph/phrases/phrase[descendant::item[@lang='%1' and contains( text(), '%2') ]] return string( $x/item[@type='segnum']/text() )").arg(dialog.writingSystem().flexString()).arg(dialog.text());
        createSearchResultDock(query);
    }
}

void MainWindow::createSearchResultDock(const QString & query)
{
    // Pop up a dockable window showing the results
    SearchQueryModel *model = new SearchQueryModel(query, mProject->textPaths(), this);
    QTreeView *tree = new QTreeView(this);
    tree->setSortingEnabled(true);
    tree->setModel(model);
    tree->sortByColumn(0, Qt::AscendingOrder);
    tree->setHeaderHidden(true);

    connect( tree, SIGNAL(activated(QModelIndex)), this, SLOT(searchResultSelected(QModelIndex)) );

    QDockWidget *dock = new QDockWidget(tr("Search Results"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setWidget(tree);
    addDockWidget(Qt::RightDockWidgetArea, dock);
}

void MainWindow::searchResultSelected( const QModelIndex & index )
{
    int lineNumber = index.data(Qt::UserRole).toInt();
    QString textName = index.parent().data(Qt::UserRole).toString();
    if( !textName.isEmpty() )
        focusTextPosition( textName, lineNumber );
}

void MainWindow::focusTextPosition( const QString & textName , int lineNumber )
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
    TextDisplayWidget* tdw = openText(textName);
    if( tdw != 0 )
        tdw->focusGlossLine(lineNumber);
}

void MainWindow::rawXQuery()
{
    // Launch a dialog requesting input
    XQueryInputDialog dialog(this);
    dialog.setWindowTitle(tr("Perform a raw XQuery on all texts"));
    if( dialog.exec() == QDialog::Accepted )
        createSearchResultDock(dialog.query());
}

void MainWindow::removeUnusedGlossItems()
{
    QString report = mProject->doDatabaseCleanup();
    QMessageBox::information(this, tr("Process complete"), report );
}

void MainWindow::sqlQueryDialog()
{
    DatabaseQueryDialog dialog(mProject->dbAdapter()->dbFilename());
    dialog.exec();
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
