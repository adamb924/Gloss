#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "project.h"
#include "databaseadapter.h"
#include "texttabwidget.h"
#include "importflextextdialog.h"
#include "writingsystem.h"
#include "mergetranslationdialog.h"
#include "generictextinputdialog.h"
#include "xquerymodel.h"
#include "xqueryinputdialog.h"
#include "databasequerydialog.h"
#include "indexsearchmodel.h"
#include "lexiconedit.h"
#include "sqltabledialog.h"
#include "focus.h"
#include "view.h"
#include "interlinearchunkeditor.h"
#include "baselinesearchreplacedialog.h"
#include "opentextdialog.h"
#include "searchform.h"
#include "viewconfigurationdialog.h"
#include "projectoptionsdialog.h"
#include "exporttextsdialog.h"
#include "syntacticvocabularydialog.h"
#include "writingsystemseditdialog.h"
#include "importplaintextdialog.h"

#include <QtWidgets>
#include <QtSql>
#include <QStringList>
#include <QComboBox>

// these may move
#include <QXmlQuery>
#include <QXmlResultItems>
#include "messagehandler.h"
#include "searchqueryview.h"
#include "mergeeafdialog.h"
#include "annotationform.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    mProject = 0;
    mInterlinearViewMenu = 0;
    mQuickViewMenu = 0;
    mSearchDock = 0;
    mAnnotationDock = 0;

    ui->setupUi(this);
    setupToolbar();

    setAppropriateWindowTitle();

    connect(ui->actionNew_Project, SIGNAL(triggered()), this, SLOT(newProject()));
    connect(ui->actionOpen_Project, SIGNAL(triggered()), this, SLOT(openProject()));
    connect(ui->actionSave_Project, SIGNAL(triggered()), this, SLOT(saveProject()));
    connect(ui->actionClose_Project, SIGNAL(triggered()), this, SLOT(closeProject()));
    connect(ui->actionClose_project_without_saving, SIGNAL(triggered()), this, SLOT(closeProjectWithoutSaving()));
    connect(ui->actionExport_texts, SIGNAL(triggered()), this, SLOT(exportTexts()) );

    connect(ui->actionOpen_text, SIGNAL(triggered()), this, SLOT(openText()));
    connect(ui->actionImport_FlexText, SIGNAL(triggered()), this, SLOT(importFlexText()));
    connect(ui->actionImport_plain_text, SIGNAL(triggered()), this, SLOT(importPlainText()));
    connect(ui->actionWriting_systems, SIGNAL(triggered()), this, SLOT(writingSystems()) );
    connect(ui->actionImport_EAF, SIGNAL(triggered()), this, SLOT(importEaf()) );

    connect(ui->actionDelete_text, SIGNAL(triggered()), this, SLOT(deleteText()) );
    connect(ui->actionMerge_translations_from_other_FlexText_file, SIGNAL(triggered()), this, SLOT(mergeTranslations()));
    connect(ui->actionBulk_merge_translations, SIGNAL(triggered()), this, SLOT(bulkMergeTranslations()));
    connect(ui->actionBulk_merge_EAF, SIGNAL(triggered()), this, SLOT(bulkMergeEaf()));
    connect(ui->actionMerge_EAF, SIGNAL(triggered()), this, SLOT(mergeEaf()));

    connect(ui->actionRaw_XQuery, SIGNAL(triggered()), this, SLOT(rawXQuery()));

    connect(ui->actionRemove_unused_gloss_items, SIGNAL(triggered()), this, SLOT(removeUnusedGlossItems()));

    connect(ui->actionPerform_a_query, SIGNAL(triggered()), this, SLOT(sqlQueryDialog()));

    connect(ui->actionSet_text_XML_from_database, SIGNAL(triggered()), this, SLOT(setTextXmlFromDatabase()));

    connect(ui->actionGenerate_report_from_texts, SIGNAL(triggered()), this, SLOT(generateTextReport()));

    connect(ui->actionCounts_of_glosses, SIGNAL(triggered()), this, SLOT(countGlosses()));
    connect(ui->actionCounts_of_text_forms, SIGNAL(triggered()), this, SLOT(countTextForms()));

    connect(ui->actionUnapproved_lines, SIGNAL(triggered()), this, SLOT(findUnapprovedLines()));
    connect(ui->actionApproved_lines, SIGNAL(triggered()), this, SLOT(findApprovedLines()));

    connect(ui->actionClose_and_save_open_texts, SIGNAL(triggered()), this, SLOT(closeOpenTexts()));
    connect(ui->actionSave_open_texts, SIGNAL(triggered()), this, SLOT(saveOpenTexts()));

    connect(ui->actionEdit_lexicon, SIGNAL(triggered()), this, SLOT(editLexicon()) );

    connect(ui->actionRebuild_index, SIGNAL(triggered()), this, SLOT(rebuildIndex()));

    connect( ui->actionBaseline_text_search_and_replace, SIGNAL(triggered()), this, SLOT(baselineSearchAndReplace()));

    connect( ui->actionSearch_dock, SIGNAL(triggered()), this, SLOT(toggleSearchDock()));
    connect( ui->actionAnnotation_dock, SIGNAL(triggered()), this, SLOT(toggleAnnotationDock()));

    connect( ui->actionConfigure_views, SIGNAL(triggered()), this, SLOT(viewConfigurationDialog()) );

    connect( ui->actionOptions, SIGNAL(triggered()), this, SLOT(projectOptions()) );

    connect( ui->actionSyntactic_elements, SIGNAL(triggered()), this, SLOT(syntacticElements()) );

    ui->actionSearch_files_instead_of_index->setCheckable(true);
    ui->actionSearch_files_instead_of_index->setChecked(false);

    setProjectActionsEnabled(false);

    addTableMenuItems();
    addMemoryModeMenuItems();
}

MainWindow::~MainWindow()
{
    if( mProject != 0 )
        delete mProject;
    delete ui;
}

void MainWindow::addTableMenuItems()
{
    QStringList tables;
    tables << "Interpretations" << "TextForms" << "Glosses" << "WritingSystems";
    addTableMenuItems(tables);

    ui->menuView_Edit_SQL_Tables->addSeparator();
    tables.clear();
    tables << "Allomorph" << "LexicalEntry" << "LexicalEntryGloss" << "LexicalEntryCitationForm" << "LexicalEntryTags" << "MorphologicalAnalysisMembers" << "GrammaticalTags";
    addTableMenuItems(tables);

    ui->menuView_Edit_SQL_Tables->addSeparator();
    tables.clear();
    tables << "SyntacticConstituents";
    addTableMenuItems(tables);

    ui->menuView_Edit_SQL_Tables->addSeparator();
    tables.clear();
    tables << "GlossIndex" << "InterpretationIndex" << "TextFormIndex";
    addTableMenuItems(tables);
}

void MainWindow::addTableMenuItems(const QStringList & tableNames)
{
    foreach(QString table, tableNames)
    {
        QAction *action = new QAction(table,ui->menuGuts);
        action->setData(table);
        ui->menuView_Edit_SQL_Tables->addAction(action);
        QActionGroup *group = new QActionGroup(this);
        group->addAction(action);
        connect(group,SIGNAL(triggered(QAction*)),this,SLOT(sqlTableView(QAction*)));
    }
}

void MainWindow::addMemoryModeMenuItems()
{
    QActionGroup *group = new QActionGroup(this);
    QAction *action;

    action = new QAction(tr("One-at-a-time"), ui->menuMemory_mode );
    action->setData(Project::OneAtATime);
    action->setCheckable(true);
    action->setChecked(true);
    ui->menuMemory_mode->addAction(action);
    group->addAction(action);

    action = new QAction(tr("Accumulate slowly"), ui->menuMemory_mode );
    action->setData(Project::AccumulateSlowly);
    action->setCheckable(true);
    ui->menuMemory_mode->addAction(action);
    group->addAction(action);

    action = new QAction(tr("Greedy / Fast"), ui->menuMemory_mode );
    action->setData(Project::GreedyFast);
    action->setCheckable(true);
    ui->menuMemory_mode->addAction(action);
    group->addAction(action);

    connect( group, SIGNAL(triggered(QAction*)), this, SLOT(setMemoryMode(QAction*)) );
}

void MainWindow::newProject()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("New") );

    if( filename.isNull() ) return;

    if( mProject != 0 )
        delete mProject;
    mProject = new Project(this);
    mProject->create(filename);

    if( writingSystems() == 0)
        return;
    if( projectOptions() == 0)
        return;
    if( viewConfigurationDialog() == 0)
        return;

    mProject->serializeConfigurationXml();

    setAppropriateWindowTitle();
    setProjectActionsEnabled(true);
    setGuiElementsFromProject();
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
            setAppropriateWindowTitle();
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
    if( mProject == 0 )
        return;
    if( !mProject->isChanged() || QMessageBox::question(this, tr("Really?"), tr("Close project without saving changes?")) == QMessageBox::Yes )
    {
        projectClose();
    }
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
    setAppropriateWindowTitle();
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
    ImportPlainTextDialog dlg(mProject->dbAdapter()->writingSystems());
    if(dlg.exec())
    {
        WritingSystem ws = dlg.writingSystem();
        QStringList files = dlg.filenames();
        if( files.count() == 1 )
        {
            if( dlg.customDelimiter() )
            {
                importPlainText( files.first() , ws , true, dlg.delimiter() );
            }
            else
            {
                importPlainText( files.first() , ws , true );
            }
        }
        else
        {
            QProgressDialog progress("Importing texts...", "Cancel", 0, files.count(), this);
            progress.setWindowModality(Qt::WindowModal);
            for(int i=0; i<files.count(); i++)
            {
                progress.setValue(i);
                if( QFile::exists(files.at(i)))
                {
                    if( dlg.customDelimiter() )
                    {
                        importPlainText( files.at(i) , ws , false, dlg.delimiter() );
                    }
                    else
                    {
                        importPlainText( files.at(i) , ws , false );
                    }
                }
                if( progress.wasCanceled() )
                {
                    break;
                }
            }
            progress.setValue(files.count());
        }
    }
}

void MainWindow::importPlainText(const QString & filepath , const WritingSystem & ws, bool openText, const QRegularExpression & re)
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

        Text *text = mProject->newText(name, ws, content, re );
        if( openText )
        {
            InterlinearChunkEditor * subWindow = new InterlinearChunkEditor(text, mProject, View::Full, 3, this);
            ui->mdiArea->addSubWindow(subWindow);
            subWindow->show();
        }
    }
}

void MainWindow::importEaf()
{
    QFileDialog dialog( this, tr("Select file(s) to import"), QString(), "*.eaf" );
    dialog.setFileMode(QFileDialog::ExistingFiles);
    if( dialog.exec() == QDialog::Accepted )
    {
        bool ok;

        QString tierId = QInputDialog::getText(this, tr("Tier ID"), tr("Enter the tier ID:"), QLineEdit::Normal, "default", &ok);
        if( ok )
        {
            WritingSystem ws = selectWritingSystem(tr("Select the baseline writing system for these texts."), &ok);
            if( ok )
            {
                QStringList files = dialog.selectedFiles();

                if( files.count() == 1 )
                {
                    if ( importEaf( files.first(), tierId, ws ) )
                    {
                        QMessageBox::information(this, tr("Eaf file imported"), tr("%1 has been successfully imported").arg(files.first()));
                    }
                    else
                    {
                        QMessageBox::information(this, tr("Error"), tr("There was an error importing %1.").arg(files.first()));
                    }
                }
                else
                {
                    int successes = 0;
                    int failures = 0;
                    QProgressDialog progress("Importing texts...", "Cancel", 0, files.count(), this);
                    progress.setWindowModality(Qt::WindowModal);
                    for(int i=0; i<files.count(); i++)
                    {
                        progress.setValue(i);
                        if( QFile::exists(files.at(i)))
                        {
                            if( importEaf( files.at(i), tierId, ws ) )
                                successes++;
                            else
                                failures++;
                        }
                        if( progress.wasCanceled() )
                            break;
                    }
                    progress.setValue(files.count());
                    QMessageBox::information(this, tr("Eaf files imported"), tr("Result of importing files: %1 successes, %2 failures or overwrite preventions").arg(successes).arg(failures));
                }
            }
        }
    }
}

bool MainWindow::importEaf(const QString & filepath, const QString & tierId, const WritingSystem & ws)
{
    QFileInfo info(filepath);
    QString name = info.baseName();

    if( !mProject->textNames().contains(name) )
    {
        QStringList result;
        QXmlQuery query(QXmlQuery::XQuery10);
        query.bindVariable("path", QVariant(QUrl::fromLocalFile(filepath).path(QUrl::FullyEncoded)));
        QString queryString = "declare variable $tier-id external; "
            "declare variable $path external; "
                              "for $x in doc($path)/ANNOTATION_DOCUMENT/TIER[@TIER_ID=$tier-id]/ANNOTATION/ALIGNABLE_ANNOTATION/ANNOTATION_VALUE "
                              "return string( $x/text() )";

        query.bindVariable("tier-id", QXmlItem(tierId) );
        query.setMessageHandler(new MessageHandler("MainWindow::importEaf"));
        query.setQuery(queryString);
        query.evaluateTo(&result);

        /// @todo This is a stupid way to do this.
        Text *text = mProject->newText(name, ws, result.join("\n"), QRegularExpression("[\\n\\r]+") );

        text->mergeEaf(filepath);

        Text::MergeEafResult mergeResult = text->mergeEaf( filepath );
        switch(mergeResult)
        {
        case Text::Success:
            break;
        case Text::MergeEafWrongNumberOfAnnotations:
            QMessageBox::information(0, tr("Failure!"), tr("The import of %1 has failed because the number of annotations is wrong (in the merge stage).").arg( filepath ));
        default:
            QMessageBox::information(0, tr("Failure!"), tr("The merge into %1 has failed.").arg( filepath ));
            break;
        }
        return true;
    }
    return false;
}


void MainWindow::importFlexText()
{
    ImportFlexTextDialog dialog(mProject->dbAdapter(),this);
    if( dialog.exec() == QDialog::Accepted )
    {
        if( QFile::exists(dialog.filename()) )
        {
            mProject->importFlexText(dialog.filename(),mProject->dbAdapter()->writingSystem(dialog.writingSystem()));
            Text *text = mProject->openedTexts()->value( Text::textNameFromPath(dialog.filename()) );
            if(text != 0)
                openTextInChunks(text->name());
        }
    }
}

void MainWindow::sqlTableView( QAction * action )
{
    QString name = action->data().toString();
    SqlTableDialog * dialog = new SqlTableDialog(name, mProject->dbAdapter());
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
    mProject->setChanged();
}

void MainWindow::setProjectActionsEnabled(bool enabled)
{
    ui->actionSave_Project->setEnabled(enabled);
    ui->actionSave_Project_As->setEnabled(enabled);
    ui->actionClose_Project->setEnabled(enabled);
    ui->actionClose_project_without_saving->setEnabled(enabled);
    ui->actionExport_texts->setEnabled(enabled);

    foreach(QAction * action , ui->menuData->actions() )
        action->setEnabled(enabled);

    foreach(QAction * action , ui->menuSearch->actions() )
        action->setEnabled(enabled);

    foreach(QAction * action , ui->menuProject->actions() )
        action->setEnabled(enabled);

    foreach(QAction * action , ui->menuGuts->actions() )
        action->setEnabled(enabled);

    ui->menuData->setEnabled(enabled);
    ui->menuGuts->setEnabled(enabled);
    ui->menuProject->setEnabled(enabled);
    ui->menuSearch->setEnabled(enabled);

    ui->toolBar->setEnabled(enabled);
}

void MainWindow::openText()
{
    if( mProject->textPaths()->count() == 0)
    {
        QMessageBox::information(this, tr("No texts"), tr("The project has no texts to open."));
        return;
    }

    OpenTextDialog dialog(mProject->textNames(), this);
    if( dialog.exec() == QDialog::Accepted )
    {
        if( dialog.linesPerScreen() == -1 )
        {
            openText( dialog.textName() );
        }
        else
        {
            InterlinearChunkEditor * ice = openTextInChunks( dialog.textName(), dialog.linesPerScreen() );
            if( ice != 0 && dialog.goToLine() > dialog.linesPerScreen() )
                ice->moveToLine( dialog.goToLine() );
        }
    }
}

TextTabWidget* MainWindow::openText(const QString & textName, const QList<Focus> & foci)
{
    Text *text;
    TextTabWidget *subWindow = 0;
    switch( mProject->openText(textName) )
    {
    case Project::Success:
        text = mProject->openedTexts()->value(textName, 0);
        if( text != 0 )
        {
            subWindow = new TextTabWidget(text, mProject, View::Full, QList<int>(), foci, this);
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

int MainWindow::writingSystems()
{
    WritingSystemsEditDialog dlg(mProject, this);
    if( dlg.exec() )
    {
        mProject->dbAdapter()->loadWritingSystems();
        mProject->setChanged();
        return 1;
    }
    return 0;
}

int MainWindow::projectOptions()
{
    ProjectOptionsDialog dialog(mProject, this);
    int retVal = dialog.exec();
    if( retVal != 0 )
    {
        mProject->setChanged();
    }
    return retVal;
}

void MainWindow::exportTexts()
{
    ExportTextsDialog dlg(mProject->textNames());
    if( dlg.exec() == QDialog::Accepted )
    {
        QDir dir(dlg.destinationFolder());
        foreach( QString textName, dlg.textNames() )
        {
            Text * text = mProject->text(textName);
            if( text != 0)
            {
                text->writeTextTo( dir.absoluteFilePath(textName+".flextext") , true, dlg.includeGlossNamespace() );
            }
            else
            {
                QMessageBox::critical(this, tr("Error"), tr("There was an error exporting %1").arg(textName));
            }
        }
    }
}

void MainWindow::syntacticElements()
{
    SyntacticVocabularyDialog dlg(mProject);
    if( dlg.exec() )
    {
        mProject->dbAdapter()->loadSyntacticTypes();
        mProject->setChanged();
    }
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
        Text *text = mProject->openedTexts()->value(dialog.text());
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

void MainWindow::searchForInterpretationById(qlonglong id)
{
    QStandardItemModel *model;
    QList<Focus> foci;
    foci << Focus(Focus::Interpretation, id );

    if( ui->actionSearch_files_instead_of_index->isChecked() )
    {
        QString query = QString("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                                "declare variable $path external; "
                                "for $x in doc($path)/document/interlinear-text/paragraphs/paragraph/phrases/phrase[descendant::word[@abg:id='%1']] "
                                "let $line-number := string( $x/item[@type='segnum']/text() ) "
                                "let $count := string( count( $x/descendant::word[@abg:id='%1'] ) ) "
                                "order by number($x/item[@type='segnum']/text()) "
                                "return   string-join( ($line-number, $count) , ',') ").arg(id);
        model = new XQueryModel(query, mProject->textPaths(), this, foci);
    }
    else
    {
        if( mProject->dbAdapter()->textIndicesShouldBeUpdated( mProject->textNames() ) )
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
                                "declare variable $path external; "
                                "for $x in doc($path)/document/interlinear-text/paragraphs/paragraph/phrases/phrase[descendant::word/item[@abg:id='%1' and @type='txt']] "
                                "let $line-number := string( $x/item[@type='segnum']/text() ) "
                                "let $count := string( count( $x/descendant::word/item[@abg:id='%1' and @type='txt'] ) ) "
                                "order by number($x/item[@type='segnum']/text()) "
                                "return   string-join( ($line-number, $count) , ',') ").arg(id);
        model = new XQueryModel(query, mProject->textPaths(), this, foci);
    }
    else
    {
        if( mProject->dbAdapter()->textIndicesShouldBeUpdated( mProject->textNames() ) )
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
                                "declare variable $path external; "
                                "for $x in doc($path)/document/interlinear-text/paragraphs/paragraph/phrases/phrase[descendant::word/item[@abg:id='%1' and @type='gls']] "
                                "let $line-number := string( $x/item[@type='segnum']/text() ) "
                                "let $count := string( count( $x/descendant::word/item[@abg:id='%1' and @type='gls'] ) ) "
                                "order by number($x/item[@type='segnum']/text()) "
                                "return   string-join( ($line-number, $count) , ',') " ).arg(id);
        model = new XQueryModel(query, mProject->textPaths(), this, foci);
    }
    else
    {
        if( mProject->dbAdapter()->textIndicesShouldBeUpdated( mProject->textNames() ) )
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
    if( mProject->dbAdapter()->textIndicesShouldBeUpdated( mProject->textNames() ) )
    {
        if( QMessageBox::Cancel == QMessageBox::information(this, tr("Patience..."), tr("Searching for lexical entries requires the index to be buildt. This is your first search, so the text index needs to be built. It will be slow this one time, and after that it will be quite fast."), QMessageBox::Ok | QMessageBox::Cancel , QMessageBox::Ok ) )
            return;
        mProject->dbAdapter()->createTextIndices( mProject->textPaths() );
    }

    QList<Focus> foci;
    QSetIterator<qlonglong> iter( mProject->dbAdapter()->lexicalEntryTextFormIds( id ) );
    while( iter.hasNext() )
        foci << Focus( Focus::TextForm , iter.next() );

    QStandardItemModel *model = new IndexSearchModel( mProject->dbAdapter()->searchIndexForLexicalEntry( id ), foci );
    createSearchResultDock(model, tr("Lexical Entry ID: %1").arg(id));
}

void MainWindow::searchForAllomorphById(qlonglong id)
{
    if( mProject->dbAdapter()->textIndicesShouldBeUpdated( mProject->textNames() ) )
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

void MainWindow::searchForText(const TextBit & bit)
{
    if( ui->actionSearch_files_instead_of_index->isChecked() )
    {
        // Do the search of the texts
        QString query = QString("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                                "declare variable $path external; "
                                "for $x in doc($path)/document/interlinear-text/paragraphs/paragraph/phrases/phrase[words/descendant::item[@lang='%1' and text()='%2']] "
                                "let $line-number := string( $x/item[@type='segnum']/text() ) "
                                "let $count := string( count( $x/descendant::item[@lang='%1' and text()='%2'] ) ) "
                                "order by number($x/item[@type='segnum']/text()) "
                                "return   string-join( ($line-number, $count) , ',') ").arg(bit.writingSystem().flexString()).arg(bit.text());

        XQueryModel *model = new XQueryModel(query, mProject->textPaths(), this);
        createSearchResultDock(model, tr("Containing exact string '%1'").arg(bit.text()) );
    }
    else
    {
        QStandardItemModel *model = new IndexSearchModel( mProject->dbAdapter()->searchIndexForText(bit) );
        createSearchResultDock(model, tr("Containing exact string '%1'").arg(bit.text()));
    }
}

void MainWindow::searchForSubstring(const TextBit & bit)
{
    if( ui->actionSearch_files_instead_of_index->isChecked() )
    {
        // Do the search of the texts
        QString query = QString("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                                "declare variable $path external; "
                                "for $x in doc($path)/document/interlinear-text/paragraphs/paragraph/phrases/phrase[words/descendant::item[@lang='%1' and contains( text(), '%2') ]] "
                                "let $line-number := string( $x/item[@type='segnum']/text() ) "
                                "let $count := string( count( $x/descendant::word[@lang='%1' and contains( text(), '%2') ] ) ) "
                                "order by number($x/item[@type='segnum']/text()) "
                                "return   string-join( ($line-number, $count) , ',') ").arg(bit.writingSystem().flexString()).arg(bit.text());

        XQueryModel *model = new XQueryModel(query, mProject->textPaths(), this);
        createSearchResultDock(model, tr("Containing substring '%1'").arg(bit.text()));
    }
    else
    {
        QStandardItemModel *model = new IndexSearchModel( mProject->dbAdapter()->searchIndexForSubstring(bit) );
        createSearchResultDock(model, tr("Containing substring '%1'").arg(bit.text()));
    }
}

void MainWindow::rebuildIndex()
{
    mProject->dbAdapter()->createTextIndices( mProject->textPaths() );
    mProject->setChanged();
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
    connect( tree, SIGNAL(requestEditLine(QString,int,QList<Focus>)), this, SLOT(focusTextPosition(QString,int,QList<Focus>)) );
//    connect( tree, SIGNAL(requestEditLineWithContext(QString,int,QList<Focus>)), this, SLOT(editLineWithContext(QString,int,QList<Focus>)) );

    QWidget *intermediateWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(intermediateWidget);
    layout->addWidget( new QLabel(reminder) );
    layout->addWidget(tree);
    intermediateWidget->setLayout(layout);

    QDockWidget *dock = new QDockWidget(tr("Search Results"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setWidget(intermediateWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    connect(mProject, SIGNAL(destroyed()), dock, SLOT(close()));
}

int MainWindow::viewConfigurationDialog()
{
    ViewConfigurationDialog dialog(mProject, this);
    int result = dialog.exec();
    if( result )
    {
        setGuiElementsFromProject();
        mProject->setChanged();
    }
    return result;
}

void MainWindow::focusTextPosition( const QString & textName , int lineNumber, const QList<Focus> & foci )
{
    QList<QMdiSubWindow*> windows = ui->mdiArea->subWindowList();

    QListIterator<QMdiSubWindow*> iter(windows);
    while(iter.hasNext())
    {
        QMdiSubWindow* w = iter.next();
        TextTabWidget* ttw = qobject_cast<TextTabWidget*>(w->widget());
        InterlinearChunkEditor* ice = qobject_cast<InterlinearChunkEditor*>(w->widget());

        if( ttw != 0 && ttw->text()->name() == textName )
        {
            ui->mdiArea->setActiveSubWindow(w);
            w->raise();
            ttw->setFocus(foci);
            return;
        }
        else if( ice != 0 && ice->text()->name() == textName )
        {
            ice->moveToLine( lineNumber );
            ice->setFocus(foci);
            w->raise();
            return;
        }
    }

    // at this point the window must not exist
    InterlinearChunkEditor * ice = openTextInChunks( textName, 3 );
    if( ice != 0 )
    {
        ice->moveToLine( lineNumber  && lineNumber > 3 );
        ice->raise();
        ice->setFocus(foci);
    }
}

void MainWindow::playSoundForLine( const QString & textName , int lineNumber )
{
    if( lineNumber == -1 )
        return;

    if( mProject->openedTexts()->contains(textName) )
    {
        mProject->openText(textName);
        Text *text = mProject->openedTexts()->value(textName, 0);
        if( text == 0)
            return;
        lineNumber--;
        text->playSoundForLine(lineNumber);
    }
    else
    {
        mProject->playLine( textName, lineNumber );
    }
}

void MainWindow::rawXQuery()
{
    // Launch a dialog requesting input
    XQueryInputDialog dialog(this);
    dialog.setWindowTitle(tr("Perform a raw XQuery on all texts"));
    if( dialog.exec() == QDialog::Accepted )
    {
        XQueryModel *model = new XQueryModel(dialog.query(), mProject->textPaths(), this);
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
    mProject->setChanged();
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
                Text *text = mProject->openedTexts()->value(textName);
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
        Text *text = mProject->openedTexts()->value(dialog.text());
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
                Text *text = mProject->openedTexts()->value(textName);
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

void MainWindow::findApprovedLines()
{
    QString query = QString("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                            "declare variable $path external; "
                            "for $x in doc($path)/document/interlinear-text/paragraphs/paragraph/phrases/phrase[count(words/word/@abg:approval-status='false')=0] "
                            "order by number($x/item[@type='segnum']/text()) "
                            "return string( $x/item[@type='segnum']/text() )");
    XQueryModel *model = new XQueryModel(query, mProject->textPaths(), this);
    createSearchResultDock(model, tr("Approved lines") );
}

void MainWindow::findUnapprovedLines()
{
    QString query = QString("declare namespace abg = 'http://www.adambaker.org/gloss.php'; "
                            "declare variable $path external; "
                            "for $x in doc($path)/document/interlinear-text/paragraphs/paragraph/phrases/phrase[exists(words/word/@abg:approval-status='false')] "
                            "order by number($x/item[@type='segnum']/text()) "
                            "return string( $x/item[@type='segnum']/text() )");
    XQueryModel *model = new XQueryModel(query, mProject->textPaths(), this);
    createSearchResultDock(model, tr("Unapproved lines") );
}

void MainWindow::closeOpenTexts()
{
    mProject->saveOpenTexts();
    mProject->closeOpenTexts( textsWithOpenWindows() );
}

void MainWindow::saveOpenTexts()
{
    mProject->saveOpenTexts();
}

QStringList MainWindow::textsWithOpenWindows()
{
    QStringList textNames;
    foreach( QMdiSubWindow * window , ui->mdiArea->subWindowList() )
        textNames << window->windowTitle();
    return textNames;
}

void MainWindow::setAppropriateWindowTitle()
{
    if( mProject == 0 )
    {
        setWindowTitle(tr("Gloss"));
    }
    else
    {
        setWindowTitle( tr("Gloss - %1").arg(QFileInfo(mProject->projectPath()).fileName()) );
    }
}

void MainWindow::editLexicon()
{
    LexiconEdit *edit = new LexiconEdit( mProject, this );
    connect( edit, SIGNAL(textFormIdSearch(qlonglong)), this, SLOT(searchForTextFormById(qlonglong)) );
    edit->show();
    mProject->setChanged();
}

void MainWindow::setGuiElementsFromProject()
{
    /// View menus
    qDeleteAll(ui->menuCurrent_view->actions());
    qDeleteAll(ui->menuCurrent_quick_view->actions());

    /// View combos
    mViewCombo->clear();
    mQuickViewCombo->clear();

    /// Texts
    mTextCombo->clear();

    if( mProject == 0 ) return;

    QActionGroup * views = new QActionGroup(this);

    for(int i=0; i<mProject->views()->count(); i++)
    {
        QAction *act = ui->menuCurrent_view->addAction( mProject->views()->at(i)->name() );
        act->setCheckable(true);
        act->setData( i );
        views->addAction(act);

        /// Combo
        mViewCombo->addItem( mProject->views()->at(i)->name() );
    }
    connect( views, SIGNAL(triggered(QAction*)), mProject, SLOT(setView(QAction*)) );

    QActionGroup * interlinearViews = new QActionGroup(this);

    for(int i=0; i<mProject->views()->count(); i++)
    {
        QAction *act = ui->menuCurrent_quick_view->addAction( mProject->views()->at(i)->name() );
        act->setCheckable(true);
        act->setData( i );
        interlinearViews->addAction(act);

        /// Combo
        mQuickViewCombo->addItem( mProject->views()->at(i)->name() );
    }
    connect( interlinearViews, SIGNAL(triggered(QAction*)), mProject, SLOT(setQuickView(QAction*)) );

    if( ui->menuCurrent_view->actions().count() > 0 )
    {
        ui->menuCurrent_view->actions().first()->setChecked(true);
        mProject->setView(ui->menuCurrent_view->actions().first());
    }

    if( ui->menuCurrent_quick_view->actions().count() > 0 )
    {
        ui->menuCurrent_quick_view->actions().first()->setChecked(true);
        mProject->setQuickView(ui->menuCurrent_quick_view->actions().first());
    }

    /// Project Texts
    mTextCombo->addItems( mProject->textNames() );

    connect( mProject, SIGNAL(currentViewChanged(int)), this, SLOT(setCurrentView(int)) );
    connect( mProject, SIGNAL(currentQuickViewChanged(int)), this, SLOT(setCurrentQuickView(int)) );
}

void MainWindow::setupToolbar()
{
    /// Texts
    mTextCombo = new QComboBox(this);
    mTextCombo->setMinimumWidth(150);
    connect(mTextCombo, SIGNAL(activated(QString)), this, SLOT(openTextInChunks(QString)));

    /// Views
    mViewCombo = new QComboBox(this);
    mQuickViewCombo = new QComboBox(this);

    mViewCombo->setMinimumWidth(100);
    mQuickViewCombo->setMinimumWidth(100);

    connect(mViewCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(viewChanged(int)) );
    connect(mQuickViewCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(quickViewChanged(int)) );

    ui->toolBar->addWidget(new QLabel("Open Text: "));
    ui->toolBar->addWidget(mTextCombo);
    ui->toolBar->addSeparator();

    ui->toolBar->addWidget(new QLabel("View: "));
    ui->toolBar->addWidget(mViewCombo);
    ui->toolBar->addSeparator();

    ui->toolBar->addWidget(new QLabel("Quick view: "));
    ui->toolBar->addWidget(mQuickViewCombo);
}

void MainWindow::viewChanged(int index)
{
    if( mProject != 0 )
    {
        mProject->setView(index);
    }
}

void MainWindow::quickViewChanged(int index)
{
    if( mProject != 0 )
    {
        mProject->setQuickView(index);
    }
}

void MainWindow::setCurrentView(int index)
{
    ui->menuCurrent_view->actions().at(index)->setChecked(true);
    mViewCombo->setCurrentIndex(index);
}

void MainWindow::setCurrentQuickView(int index)
{
    ui->menuCurrent_quick_view->actions().at(index)->setChecked(true);
    mQuickViewCombo->setCurrentIndex(index);
}

InterlinearChunkEditor * MainWindow::openTextInChunks(const QString & textName, int linesPerScreen)
{
    Text *text;
    InterlinearChunkEditor * subWindow = 0;
    switch( mProject->openText(textName) )
    {
    case Project::Success:
        text = mProject->openedTexts()->value(textName, 0);
        if( text != 0 )
        {
            subWindow = new InterlinearChunkEditor(text, mProject, View::Full, linesPerScreen, this);
            ui->mdiArea->addSubWindow(subWindow);
            subWindow->show();
        }
        return subWindow;
        break;
    case Project::FileNotFound:
        QMessageBox::critical(this, tr("Error opening file"), tr("Sorry, the text %1 could not be opened. The filename %2 could not be found.").arg(textName).arg(mProject->filepathFromName(textName)));
        return 0;
        break;
    case Project::XmlReadError:
    default:
        QMessageBox::critical(this, tr("Error opening file"), tr("Sorry, the text %1 could not be opened. There was a problem reading the XML.").arg(textName));
        return 0;
        break;
    }
}

void MainWindow::setMemoryMode( QAction * action )
{
    if( mProject == 0 )
        return;
    mProject->setMemoryMode( (Project::MemoryMode)action->data().toInt() );
}

void MainWindow::baselineSearchAndReplace()
{
    if( mProject->memoryMode() != Project::GreedyFast )
    {
        int result = QMessageBox::question(this, tr("Gloss"), tr("This feature really only makes sense in Greedy/Fast memory mode. Do you want to switch to that? (Note that it will likely take some time to open all of your texts.)"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes );
        switch( result )
        {
        case QMessageBox::Yes:
            mProject->setMemoryMode( Project::GreedyFast );
            break;
        case QMessageBox::No:
            break;
        case QMessageBox::Cancel:
        default:
            return;
            break;
        }
    }

    QHashIterator<QString,Text*> iter( *mProject->openedTexts() );
    if( iter.hasNext() )
    {
        iter.next();

        BaselineSearchReplaceDialog dialog(mProject->dbAdapter(), iter.value()->baselineWritingSystem(), this);
        if( dialog.exec() == QDialog::Accepted )
        {
            mProject->baselineSearchReplace( dialog.searchString() , dialog.replaceString() );
            QMessageBox::information(this, tr("Complete"), tr("The search-and-replace option is complete."));
        }
    }
    else
    {
        QMessageBox::information(this, tr("Complete"), tr("There were no open texts on which to perform the operation."));
    }
}

void MainWindow::toggleSearchDock()
{
    if( mSearchDock != 0 )
        delete mSearchDock;

    if( mProject->dbAdapter()->textIndicesShouldBeUpdated( mProject->textNames() ) )
    {
        if( QMessageBox::question(this, tr("Build indices?"), tr("It looks like more texts have been added since the index was build. Shall I rebuild the index?")) == QMessageBox::Yes )
        {
            mProject->dbAdapter()->createTextIndices( mProject->textPaths() );
        }
    }

    SearchForm * searchForm = new SearchForm(mProject->dbAdapter(), this);
    searchForm->setXmlTextWarning( ui->actionSearch_files_instead_of_index->isChecked() );

    connect( searchForm, SIGNAL(searchForInterpretationById(qlonglong)), this, SLOT(searchForInterpretationById(qlonglong)) );
    connect( searchForm, SIGNAL(searchForTextFormById(qlonglong)), this, SLOT(searchForTextFormById(qlonglong)) );
    connect( searchForm, SIGNAL(searchForGlossById(qlonglong)), this, SLOT(searchForGlossById(qlonglong)) );
    connect( searchForm, SIGNAL(searchForLexicalEntryById(qlonglong)), this, SLOT(searchForLexicalEntryById(qlonglong)) );
    connect( searchForm, SIGNAL(searchForAllomorphById(qlonglong)), this, SLOT(searchForAllomorphById(qlonglong)) );
    connect( searchForm, SIGNAL(searchForText(TextBit)), this, SLOT(searchForText(TextBit)) );
    connect( searchForm, SIGNAL(searchForSubstring(TextBit)), this, SLOT(searchForSubstring(TextBit)) );
    connect( ui->actionSearch_files_instead_of_index, SIGNAL(toggled(bool)), searchForm, SLOT(setXmlTextWarning(bool)) );

    mSearchDock = new QDockWidget(tr("Search"), this);
    mSearchDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    mSearchDock->setWidget(searchForm);
    addDockWidget(Qt::RightDockWidgetArea, mSearchDock);

    connect(mProject, SIGNAL(destroyed()), mSearchDock, SLOT(close()));
}

void MainWindow::toggleAnnotationDock()
{
    if( mAnnotationDock != 0 )
        delete mAnnotationDock;

    Text * text = textOfCurrentSubWindow();
    if( text == 0 )
        return;

    AnnotationForm * annotationForm = new AnnotationForm(text, mProject, this);

    mAnnotationDock = new QDockWidget(text->name(), this);
    mAnnotationDock->setWidget(annotationForm);
    addDockWidget(Qt::BottomDockWidgetArea, mAnnotationDock);

    connect( annotationForm, SIGNAL(focusTextPosition(QString,int,QList<Focus>)), this, SLOT(focusTextPosition(QString,int,QList<Focus>)));
    connect( text, SIGNAL(destroyed()), mAnnotationDock, SLOT(close()) );
}

Text * MainWindow::textOfCurrentSubWindow()
{
    QMdiSubWindow * w = ui->mdiArea->activeSubWindow();
    if( w == 0 )
        return 0;

    TextTabWidget* tdw = qobject_cast<TextTabWidget*>(w->widget());
    if( tdw != 0 )
    {
        return tdw->text();
    }

    InterlinearChunkEditor * ice = qobject_cast<InterlinearChunkEditor*>(w->widget());
    if( ice != 0 )
    {
        return ice->text();
    }
    return 0;
}
