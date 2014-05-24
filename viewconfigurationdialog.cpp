#include "viewconfigurationdialog.h"
#include "ui_viewconfigurationdialog.h"

#include <QtDebug>
#include <QItemSelection>

#include "project.h"
#include "view.h"
#include "tab.h"

#include "viewsmodel.h"
#include "tabsmodel.h"
#include "itemsmodel.h"
#include "phrasalglossesmodel.h"
#include "writingsystemcombo.h"
#include "databaseadapter.h"
#include "writingsystemdialog.h"
#include "itemeditdialog.h"

ViewConfigurationDialog::ViewConfigurationDialog(Project *project, QWidget *parent) :
    mProject(project), QDialog(parent),
    ui(new Ui::ViewConfigurationDialog)
{
    ui->setupUi(this);

//    ui->viewView->setDragDropMode( QAbstractItemView::InternalMove  );
    ui->viewView->setDragDropMode( QAbstractItemView::DragDrop );
    ui->viewView->setDragEnabled(true);
    ui->viewView->setAcceptDrops(true);
    ui->viewView->setDropIndicatorShown(true);

    mViewModel = new ViewsModel(mProject);
    ui->viewView->setModel( mViewModel );

    mTabsModel = 0;
    mItemsModel = 0;
    mPhrasalGlossesModel = 0;

    mView = 0;
    mTab = 0;

    connect(ui->addView, SIGNAL(clicked()), this, SLOT(addView()) );
    connect(ui->removeView, SIGNAL(clicked()), this, SLOT(removeView()) );

    connect(ui->addTab, SIGNAL(clicked()), this, SLOT(addTab()) );
    connect(ui->removeTab, SIGNAL(clicked()), this, SLOT(removeTab()) );

    connect(ui->addItem, SIGNAL(clicked()), this, SLOT(addItem()) );
    connect(ui->removeItem, SIGNAL(clicked()), this, SLOT(removeItem()) );
    connect(ui->itemView, SIGNAL(activated(QModelIndex)), this, SLOT(editItem(QModelIndex)) );

    connect(ui->addPhrasalGloss, SIGNAL(clicked()), this, SLOT(addPhrasalGloss()) );
    connect(ui->removePhrasalGloss, SIGNAL(clicked()), this, SLOT(removePhrasalGloss()) );
    connect(ui->phrasalGlossView, SIGNAL(activated(QModelIndex)), this, SLOT(editPhrasalGloss(QModelIndex)) );

    connect(ui->viewView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(viewChanged(QItemSelection,QItemSelection)));
    connect(ui->itemWritingSystemsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(indexLanguageChanged(int)));

    connect(ui->viewUp, SIGNAL(clicked()), this, SLOT(viewUp()));
    connect(ui->viewDown, SIGNAL(clicked()), this, SLOT(viewDown()));
    connect(ui->tabUp, SIGNAL(clicked()), this, SLOT(tabUp()));
    connect(ui->tabDown, SIGNAL(clicked()), this, SLOT(tabDown()));
    connect(ui->itemUp, SIGNAL(clicked()), this, SLOT(itemUp()));
    connect(ui->itemDown, SIGNAL(clicked()), this, SLOT(itemDown()));
    connect(ui->phrasalGlossUp, SIGNAL(clicked()), this, SLOT(phrasalGlossUp()));
    connect(ui->phrasalGlossDown, SIGNAL(clicked()), this, SLOT(phrasalGlossDown()));
}

ViewConfigurationDialog::~ViewConfigurationDialog()
{
    delete ui;
}

void ViewConfigurationDialog::addView()
{
    mViewModel->insertRow( mViewModel->rowCount() );
}

void ViewConfigurationDialog::removeView()
{
    qDebug() << ui->viewView->selectionModel()->selectedRows().count();
}

void ViewConfigurationDialog::addTab()
{

}

void ViewConfigurationDialog::removeTab()
{

}

void ViewConfigurationDialog::addItem()
{
    if( mTab == 0 ) return;
    ItemEditDialog dialog(mProject->dbAdapter()->writingSystems(), this);
    dialog.exec();
    if( dialog.result() == QDialog::Accepted )
    {
        mItemsModel->addItem( dialog.type(), currentWritingSystem() );
    }
}

void ViewConfigurationDialog::removeItem()
{
    if( mTab == 0 || ui->itemView->selectionModel()->selectedIndexes().count() == 0 ) return;
    mItemsModel->removeItem( currentWritingSystem(), ui->itemView->selectionModel()->selectedIndexes().first() );
}

void ViewConfigurationDialog::addPhrasalGloss()
{
    if( mPhrasalGlossesModel == 0 ) return;
    WritingSystemDialog dialog(mProject->dbAdapter()->writingSystems(), this);
    dialog.exec();
    if( dialog.result() == QDialog::Accepted )
    {
        mPhrasalGlossesModel->addPhrasalGloss( dialog.writingSystem() );
    }
}

void ViewConfigurationDialog::removePhrasalGloss()
{
    if( mPhrasalGlossesModel == 0 || ui->phrasalGlossView->selectionModel()->selectedRows().isEmpty() ) return;
    mPhrasalGlossesModel->removePhrasalGloss( ui->phrasalGlossView->selectionModel()->selectedRows().first().row() );
}

void ViewConfigurationDialog::viewChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    if( ! selected.indexes().isEmpty() )
    {
        int currentView = selected.indexes().first().row();
        if( mTabsModel != 0 )
        {
            delete mTabsModel;
        }

        mView = mProject->views()->at( currentView );
        mTabsModel = new TabsModel( mView );
        ui->tabView->setModel(mTabsModel);
        connect(ui->tabView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(tabChanged(QItemSelection,QItemSelection)) );
    }
}

void ViewConfigurationDialog::tabChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    if( ! selected.indexes().isEmpty() &&  !ui->viewView->selectionModel()->selectedRows().isEmpty() && mView != 0 )
    {
        int currentTab = selected.indexes().first().row();
        mTab = mView->tabs()->at(currentTab);

        ui->itemWritingSystemsCombo->clear();
        QList<WritingSystem> availableWS = mTab->interlinearLines().keys();
        for(int i=0; i<availableWS.count(); i++)
        {
            ui->itemWritingSystemsCombo->addItem( availableWS.at(i).name() );
        }

        WritingSystem currentWS = currentWritingSystem();
        if( mItemsModel != 0 )
        {
            delete mItemsModel;
        }
        mItemsModel = new ItemsModel(mTab,currentWS);
        ui->itemView->setModel(mItemsModel);


        if( mPhrasalGlossesModel != 0 )
        {
            delete mPhrasalGlossesModel;
        }
        mPhrasalGlossesModel = new PhrasalGlossesModel(mTab);
        ui->phrasalGlossView->setModel(mPhrasalGlossesModel);

        // @todo connections here
    }
}

void ViewConfigurationDialog::indexLanguageChanged(int index)
{
    WritingSystem currentWS = currentWritingSystem();
    if( mItemsModel != 0 )
    {
        delete mItemsModel;
    }
    mItemsModel = new ItemsModel(mTab,currentWS);
    ui->itemView->setModel(mItemsModel);
    // @todo connections here
}

void ViewConfigurationDialog::editPhrasalGloss(const QModelIndex &index)
{
    WritingSystemDialog dialog(mProject->dbAdapter()->writingSystems(), this);
    dialog.exec();
    if( dialog.result() == QDialog::Accepted )
    {
        mPhrasalGlossesModel->editPhrasalGloss( index, dialog.writingSystem() );
    }
}

void ViewConfigurationDialog::editItem(const QModelIndex &index)
{
    if( mTab == 0 ) return;
    ItemEditDialog dialog(mProject->dbAdapter()->writingSystems(), this);
    dialog.setType( mTab->interlinearLines().value( currentWritingSystem() )->at( index.row() ) );
    dialog.exec();
    if( dialog.result() == QDialog::Accepted )
    {
        mItemsModel->editItem(currentWritingSystem(), index, dialog.type() );
    }
}

WritingSystem ViewConfigurationDialog::currentWritingSystem() const
{
    if( ui->itemWritingSystemsCombo->count() == 0 || mTab == 0 )
    {
        return WritingSystem();
    }
    else
    {
        return mTab->interlinearLines().keys().at( ui->itemWritingSystemsCombo->currentIndex() );
    }
}

void ViewConfigurationDialog::viewUp()
{
    if(mViewModel == 0 || ui->viewView->selectionModel()->selectedRows().count() == 0 ) return;
    mViewModel->moveUp( ui->viewView->selectionModel()->selectedRows().first().row() );
}

void ViewConfigurationDialog::viewDown()
{
    if(mViewModel == 0 || ui->viewView->selectionModel()->selectedRows().count() == 0 ) return;
    mViewModel->moveDown( ui->viewView->selectionModel()->selectedRows().first().row() );
}

void ViewConfigurationDialog::tabUp()
{
    if(mTabsModel == 0 || ui->tabView->selectionModel()->selectedRows().count() == 0 ) return;
    mTabsModel->moveUp( ui->tabView->selectionModel()->selectedRows().first().row() );
}

void ViewConfigurationDialog::tabDown()
{
    if(mTabsModel == 0 || ui->tabView->selectionModel()->selectedRows().count() == 0 ) return;
    mTabsModel->moveDown( ui->tabView->selectionModel()->selectedRows().first().row() );
}

void ViewConfigurationDialog::itemUp()
{
    if(mItemsModel == 0 || ui->itemView->selectionModel()->selectedRows().count() == 0 ) return;
    mItemsModel->moveUp( currentWritingSystem(), ui->itemView->selectionModel()->selectedRows().first().row() );
}

void ViewConfigurationDialog::itemDown()
{
    if(mItemsModel == 0 || ui->itemView->selectionModel()->selectedRows().count() == 0 ) return;
    mItemsModel->moveDown( currentWritingSystem(), ui->itemView->selectionModel()->selectedRows().first().row() );
}

void ViewConfigurationDialog::phrasalGlossUp()
{
    if(mPhrasalGlossesModel == 0 || ui->phrasalGlossView->selectionModel()->selectedRows().count() == 0 ) return;
    mPhrasalGlossesModel->moveUp( ui->phrasalGlossView->selectionModel()->selectedRows().first().row() );
}

void ViewConfigurationDialog::phrasalGlossDown()
{
    if(mPhrasalGlossesModel == 0 || ui->phrasalGlossView->selectionModel()->selectedRows().count() == 0 ) return;
    mPhrasalGlossesModel->moveDown( ui->phrasalGlossView->selectionModel()->selectedRows().first().row() );
}
