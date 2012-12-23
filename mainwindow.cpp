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

#include <QtGui>
#include <QtSql>

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
    QString table;
    QStringList tables;
    tables << "Interpretations" << "TextForms" << "Glosses" << "MorphologicalAnalyses" << "WritingSystems" << "InterlinearTextLine" << "PhrasalGlossLine";
    foreach(table, tables)
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

WritingSystem MainWindow::selectWritingSystem(bool *ok)
{
    QList<WritingSystem> list = mProject->dbAdapter()->writingSystems();
    QStringList names;
    for(int i=0; i<list.count(); i++)
        names << list.at(i).name();
    QString item = QInputDialog::getItem(this, tr("Select writing system"), tr("Select the baseline writing system for these texts."), names, 0, false, ok );
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
        WritingSystem ws = selectWritingSystem(&ok);
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
    ui->actionAdd_text->setEnabled(enabled);
    ui->actionImport_FlexText->setEnabled(enabled);
    ui->actionSave_Project->setEnabled(enabled);
    ui->actionClose_Project->setEnabled(enabled);
    ui->actionWriting_systems->setEnabled(enabled);
    ui->actionClose_project_without_saving->setEnabled(enabled);
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

void MainWindow::openText(const QString & textName)
{
    if( ! mProject->openText(textName) )
    {
        QMessageBox::critical(this, tr("Error opening file"), tr("Sorry, the text %1 could not be opened.").arg(textName));
        return;
    }
    Text *text = mProject->texts()->value(textName, 0);
    if( text == 0 )
    {
        QMessageBox::critical(this, tr("Error opening file"), tr("Sorry, the text %1 could not be opened.").arg(textName));
    }
    else
    {
        TextDisplayWidget *subWindow = new TextDisplayWidget(text, mProject, this);
        ui->mdiArea->addSubWindow(subWindow);
        subWindow->show();
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
        text->mergeTranslation( dialog.filename() , dialog.writingSystem() );
        mProject->closeText(text);
    }
}
