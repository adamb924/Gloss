#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "project.h"
#include "textdisplaywidget.h"

#include <QtGui>

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

    connect(ui->actionAdd_text, SIGNAL(triggered()), this, SLOT(addText()));

    ui->menuData->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newProject()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("New") );

    if( !filename.isNull() )
    {
        if( mProject != 0 )
            delete mProject;
        mProject = new Project();
        mProject->initialize(filename);

        setWindowTitle( tr("Gloss - %1").arg(filename) );
        ui->menuData->setEnabled(true);
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
    }
}

void MainWindow::saveProject()
{

}

void MainWindow::saveProjectAs()
{

}

void MainWindow::addText()
{
    TextDisplayWidget *tmp = new TextDisplayWidget(mProject, this);
    ui->mdiArea->addSubWindow(tmp);
    tmp->show();
}
