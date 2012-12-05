#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "project.h"
#include "databaseadapter.h"
#include "textdisplaywidget.h"
#include "newtextdialog.h"
#include "importflextextdialog.h"

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
    connect(ui->actionSave_Project_As, SIGNAL(triggered()), this, SLOT(saveProjectAs()));
    connect(ui->actionClose_Project, SIGNAL(triggered()), this, SLOT(closeProject()));

    connect(ui->actionOpen_text, SIGNAL(triggered()), this, SLOT(openText()));
    connect(ui->actionAdd_text, SIGNAL(triggered()), this, SLOT(addBlankText()));
    connect(ui->actionImport_FlexText, SIGNAL(triggered()), this, SLOT(importFlexText()));

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
    tables << "Interpretations" << "TextForms" << "MorphologicalAnalyses" << "Glosses" << "WritingSystems" << "GlossLines";
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
        mProject->readFromFile(filename);

        setWindowTitle( tr("Gloss - %1").arg(filename) );
        setProjectActionsEnabled(true);
    }
}

void MainWindow::saveProject()
{

}

void MainWindow::saveProjectAs()
{

}

void MainWindow::closeProject()
{
    if( mProject == 0 )
        return;
    if( maybeSave() )
        projectClose();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if( mProject == 0 )
    {
        event->accept();
        return;
    }
    if( maybeSave() )
        projectClose();
}

void MainWindow::projectClose()
{
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
        Text *text = mProject->newBlankText(dialog.name(), dialog.writingSystem());
        TextDisplayWidget *subWindow = new TextDisplayWidget(text, mProject, this);
        ui->mdiArea->addSubWindow(subWindow);
        subWindow->show();
    }
}

void MainWindow::sqlTableView( QAction * action )
{
    QString name = action->data().toString();
    QSqlTableModel *model = new QSqlTableModel(this,*mProject->dbAdapter()->db());
    model->setTable(name);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    QTableView *view = new QTableView;
    view->setModel(model);
    view->setSortingEnabled(true);
    view->setWindowTitle(name);
    view->show();
}

void MainWindow::importFlexText()
{
    ImportFlexTextDialog dialog(mProject,this);
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

void MainWindow::setProjectActionsEnabled(bool enabled)
{
    ui->menuData->setEnabled(enabled);
    ui->menuGuts->setEnabled(enabled);
    ui->actionAdd_text->setEnabled(enabled);
    ui->actionImport_FlexText->setEnabled(enabled);
    ui->actionSave_Project->setEnabled(enabled);
    ui->actionSave_Project_As->setEnabled(enabled);
    ui->actionClose_Project->setEnabled(enabled);
}

void MainWindow::openText()
{
    if( mProject->textPaths()->count() == 0)
    {
        QMessageBox::information(this, tr("No texts"), tr("The project has no texts to open."));
        return;
    }

    QStringList texts;
    QFileInfo info;

    for(int i=0; i<mProject->textPaths()->count(); i++)
    {
        info.setFile(mProject->textPaths()->at(i));
        texts << info.baseName();
    }
    bool ok;
    QString whichText = QInputDialog::getItem( this, tr("Select text"), tr("Select the text to open"), texts, 0, false, &ok );
    if( ok )
        openText(whichText);
}

void MainWindow::openText(const QString & textName)
{
    mProject->openText(textName);
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
