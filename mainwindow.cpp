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

    connect(ui->actionAdd_text, SIGNAL(triggered()), this, SLOT(addBlankText()));
    connect(ui->actionImport_FlexText, SIGNAL(triggered()), this, SLOT(importFlexText()));

    ui->menuData->setEnabled(false);
    ui->menuGuts->setEnabled(false);

    addTableMenuItems();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addTableMenuItems()
{
    QString table;
    QStringList tables;
    tables << "Interpretations" << "PhoneticForms" << "MorphologicalAnalyses" << "Glosses" << "WritingSystems" << "GlossLines";
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
        ui->menuData->setEnabled(true);
        ui->menuGuts->setEnabled(true);
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
        ui->menuData->setEnabled(true);
        ui->menuGuts->setEnabled(true);
    }
}

void MainWindow::saveProject()
{

}

void MainWindow::saveProjectAs()
{

}

void MainWindow::addBlankText()
{
    NewTextDialog dialog( mProject->dbAdapter()->writingSystems(), this);
    if( dialog.exec() == QDialog::Accepted )
    {
        Text *text = mProject->newBlankText(dialog.name(), dialog.baselineMode(), mProject->dbAdapter()->writingSystem(dialog.writingSystem()));
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
            QFile *file = new QFile(dialog.filename());
            Text *text = mProject->textFromFlexText(file,dialog.baselineMode(),mProject->dbAdapter()->writingSystem(dialog.writingSystem()));
            TextDisplayWidget *subWindow = new TextDisplayWidget(text, mProject, this);
            ui->mdiArea->addSubWindow(subWindow);
            subWindow->show();
        }
        else
        {
            // TODO error message
        }
    }
}
